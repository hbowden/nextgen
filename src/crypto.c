

/**
 * Copyright (c) 2015, Harrison Bowden, Secure Labs, Minneapolis, MN
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice 
 * and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **/

#include "crypto.h"
#include "io.h"
#include "memory.h"
#include "nextgen.h"
#include "openssl/sha.h"
#include "openssl/crypto.h"
#include "openssl/rand.h"
#include "openssl/bn.h"
#include "openssl/evp.h"
#include "openssl/engine.h"

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

static int (*rand_range_pointer)(unsigned int range, unsigned int *number);

static int software_prng;

static int rand_range_no_crypto(unsigned int range, unsigned int *number)
{
    *number = (unsigned int) rand() % range + 1;
    return 0;
}

int using_hardware_prng(void)
{
    return software_prng;
}

int rand_bytes(char **buf, unsigned int length)
{
    int rtrn = RAND_bytes((unsigned char *)*buf, (int) length);
    if(rtrn != 1)
    {
        output(STD, "Can't get random bytes\n");
        return -1;
    }

    return 0;
}

static int rand_range_crypto(unsigned int range, unsigned int *number)
{
    BIGNUM *random, *range1;

    random = BN_new();
    if(random == NULL)
    {
    	output(ERROR, "Can't init bignum struct\n");
    	return -1;
    }

    range1 = BN_new();
    if(random == NULL)
    {
    	output(ERROR, "Can't init bignum struct\n");
    	return -1;
    }

    int rtrn = BN_set_word(range1, range);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't set range\n");
    	return -1;
    }

    rtrn = BN_rand_range(random, range1);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't get random range\n");
    	return -1;
    }

    char *buf = BN_bn2dec(random);
    if(buf == NULL)
    {
    	output(ERROR, "Can't convert random number\n");
    	return -1;
    }

    *number = (unsigned int)strtol(buf, NULL, 10);

    return 0;
}

static int setup_rand_range(char *method)
{
    if(strncmp(method, "no-crypto", 10) == 0)
    {
        rand_range_pointer = &rand_range_no_crypto;
    }
    else if(strncmp(method, "crypto", 6) == 0)
    {
    	rand_range_pointer = &rand_range_crypto;
    }
    else
    {
    	output(ERROR, "Bad argument to crypto option, try again\n");
    	return -1;
    }
    
    return 0;
}

int rand_range(unsigned int range, unsigned int *number)
{
    return rand_range_pointer(range, number);
}

static int setup_hardware_acceleration(void)
{

#ifndef MAC_OSX
    ENGINE_load_rdrand();
#endif

    ENGINE* engine = ENGINE_by_id("rdrand");
    if(engine == NULL)
    {
    	output(STD, "No intel random number generator detected\n");
    	return 1;
    }

    /* Note. openssl use's zero for it's exit code. */
    int rtrn = ENGINE_init(engine);
    if(rtrn == 0)
    {
        output(ERROR, "Can't init crypto engine\n");
        return -1;
    }

    rtrn = ENGINE_set_default(engine, ENGINE_METHOD_RAND);
    if(rtrn == 0)
    {
        output(ERROR, "Can't set default method\n");
        return -1;
    }
    
    return 0;
}

int sha512(char *in, char **out)
{
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX ctx;
    int i, rtrn;
    
    rtrn = SHA512_Init(&ctx);
    if(rtrn < 0)
    {
        output(ERROR, "Sha init\n");
        return -1;
    }
    
    rtrn = SHA512_Update(&ctx, in, strlen(in));
    if(rtrn < 0)
    {
        output(ERROR, "Can't update input string\n");
        return -1;
    }
    
    rtrn = SHA512_Final(hash, &ctx);
    if(rtrn < 0)
    {
        output(ERROR, "Can't do this\n");
        return -1;
    }
    
    *out = malloc((SHA512_DIGEST_LENGTH * 2) + 1);
    if(*out == NULL)
    {
        output(ERROR, "malloc: %s\n", strerror(errno));
        return -1;
    }
    
    for(i = 0; i < SHA512_DIGEST_LENGTH; i++)
    {
        sprintf(*out + (i * 2), "%02x", hash[i]);
    }

    return 0;
}

int sha256(char *in, char **out)
{
    /* Declarations */
    int rtrn, i;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    
    rtrn =  SHA256_Init(&sha256);
    if(rtrn < 0)
    {
        output(ERROR, "Sha Init Error\n");
        return -1;
    }
    
    rtrn = SHA256_Update(&sha256, in, strlen(in));
    if(rtrn < 0)
    {
        output(ERROR, "Sha Update Error\n");
        return -1;
    }
    
    rtrn = SHA256_Final(hash, &sha256);
    if(rtrn < 0)
    {
        output(ERROR, "Sha Final Error\n");
        return -1;
    }

    *out = malloc((SHA256_DIGEST_LENGTH * 2) + 1);
    if(*out == NULL)
    {
        output(ERROR, "malloc: %s\n", strerror(errno));
        return -1;
    }
    
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(*out + (i * 2), "%02x", hash[i]);
    }
    
    return 0;
}

/*  */
int seed_prng(void)
{
    /* The variables used in seed_prng(). */
    int rtrn;
    int random_fd auto_close = 0;
    unsigned int i, loops, buf_len;
    char *random_buffer auto_clean = NULL;
    char *hash auto_clean = NULL;
    ssize_t ret;
    
    /* Allocate a buffer thats 6000 bytes long, this is
     where we will put random stuff from /dev/urandom. */
    random_buffer = malloc(6000);
    if(random_buffer == NULL)
    {
        output(ERROR, "malloc: %s\n", strerror(errno));
        return -1;
    }
    
    /* Open /dev/urandom/ */
    random_fd = open("/dev/urandom", O_RDONLY);
    if(random_fd < 0)
    {
        output(ERROR, "open '/dev/urandom': %s\n", strerror(errno));
        return -1;
    }
    
    /* Now lets read 5998 bytes from /dev/urandom  */
    ret = read(random_fd, random_buffer, 5998);
    if(ret != 5998)
    {
        output(ERROR, "read: %s\n", strerror(errno));
        return -1;
    }
    
    /* NULL terminate buffer. */
    random_buffer[5999] = '\0';
    
    /* Hash the junk from /dev/urandom with sha512. */
    rtrn = sha512(random_buffer, &hash);
    if(rtrn < 0)
    {
        output(ERROR, "can't SHA512 buffer\n");
        return -1;
    }
    
    /* Add 64 bytes of prng state. */
    RAND_add(hash, 128, 64);
    
    /* Lets pick a number in the range of zero to 256 and
     we will use that as the number of loops to do. */
    rtrn = rand_range(256, &loops);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }
    
    /* Now we enter a loop, which adds more entrophy to the prng. */
    for(i = 0; i < loops; i++)
    {
        /* Generate a random buffer length. */
        rtrn = rand_range(4096, &buf_len);
        if(rtrn < 0)
        {
            output(ERROR, "Can't generate random number\n");
            return -1;
        }

        /* Create a random buffer with the random buf length. */
        random_buffer = realloc(random_buffer, (size_t)buf_len + 1);
        if(random_buffer == NULL)
        {
            output(ERROR, "realloc: $s\n", strerror(errno));
            return -1;
        }
        
        /* Read from /dev/urandom. */
        ret = read(random_fd, random_buffer, (size_t)buf_len);
        if(ret != buf_len)
        {
            output(ERROR, "read: %s\n", strerror(errno));
            return -1;
        }
        
        /* NULL terminate the randomBuffer. */
        random_buffer[buf_len] = '\0';
        
        /* Hash the randomBuffer. */
        rtrn = sha512(random_buffer, &hash);
        if(rtrn < 0)
        {
            output(ERROR, "can't SHA512 buffer\n");
            return -1;
        }
        
        /* Add 64 bytes of prng state. */
        RAND_add(hash, 128, 64);
    }
    
    return 0;
}

int setup_crypto_module(void)
{
    output(STD, "Setting up crypto\n");

    int rtrn;

    /* Init openssl/libressl library. */
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();

    /* Check if the user want's to use a non crypto graphic random number generator. */
    if(map->crypto_flag == TRUE)
    {
        /* Pass the crypto method the user want's to setup_rand_range(). */
        rtrn = setup_rand_range(map->crypto_method);
        if(rtrn < 0)
        {
            output(ERROR,"Can't setup random range function.\n");
            return -1;
        }
    }
    else
    {
        /* We default to a crytographic random number generator. */
        rtrn = setup_rand_range((char *)"crypto");
        if(rtrn < 0)
        {
            output(ERROR,"Can't setup random range function.\n");
            return -1;
        }

        /* Check for hardware crypto accelators and use them if present, otherwise use
        /dev/urandom .*/
        rtrn = setup_hardware_acceleration();
        switch(rtrn)
        {
            case 0:
                output(STD, "Using hardware crypto PRNG\n");
                software_prng = FALSE;
                break;

            case 1: 
                output(STD, "Switching to /dev/urandom \n");  
                output(STD, "Seeding PRNG\n"); 
                software_prng = TRUE; 
                return seed_prng();

            default: 
                output(ERROR, "Error while trying to setup hardware acceleration\n"); 
                return -1;
        }  
    }
    
    return 0;
}

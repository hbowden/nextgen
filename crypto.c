

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
#include "utils.h"
#include "nextgen.h"

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/engine.h>

static int (*rand_range_pointer)(unsigned int range, int *number);

static int rand_range_no_crypto(unsigned int range, int *number)
{
    *number = (int) rand() % range + 1;
    return 0;
}

static int rand_range_crypto(unsigned int range, int *number)
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
    if(*number > (int) range || *number < 0)
    {
    	output(ERROR, "Can't convert to int.");
    	return -1;
    }

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

int rand_range(unsigned int range, int *number)
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
        printf("Can't set default method\n");
        return -1;
    }
    
    return 0;
}

static int sha512(char *input, char **output)
{
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX ctx;
    int i, rtrn;
    
    rtrn = SHA512_Init(&ctx);
    if(rtrn < 0)
    {
        printf("Sha init\n");
        return -1;
    }
    
    rtrn = SHA512_Update(&ctx, input, strlen(input));
    if(rtrn < 0)
    {
        printf("Can't update input string\n");
        return -1;
    }
    
    rtrn = SHA512_Final(hash, &ctx);
    if(rtrn < 0)
    {
        printf("Can't do this\n");
        return -1;
    }
    
    *output = malloc((SHA512_DIGEST_LENGTH * 2) + 1);
    if(*output == NULL)
    {
        printf("malloc");
        return -1;
    }
    
    for(i = 0; i < SHA512_DIGEST_LENGTH; i++)
    {
        sprintf(*output + (i * 2), "%02x", hash[i]);
    }

    return 0;
}

/*  */
int seed_prng(void)
{
    output(STD, "Seeding PRNG\n");

    /* The variables used in seedPrng(). */
    int rtrn, randomFd;
    unsigned int bufLen, iterations, i;
    char *randomBuffer, *hash;
    ssize_t ret;
    
    /* Allocate a buffer thats 6000 bytes long, this is
     where we will put random stuff from /dev/urandom. */
    randomBuffer = malloc(6000);
    if(randomBuffer == NULL)
    {
        output(ERROR, "malloc: %s\n", strerror(errno));
        return -1;
    }
    
    /* Lets open /dev/urandom/ */
    randomFd = open("/dev/urandom", O_RDONLY);
    if(randomFd < 0)
    {
        output(ERROR, "open 'dev/urandom': %s\n", strerror(errno));
        return -1;
    }
    
    /* Now lets read 5999 bytes from /dev/urandom  */
    ret = read(randomFd, randomBuffer, 5999);
    if(ret != 5999)
    {
        output(ERROR, "read: %s\n", strerror(errno));
        return -1;
    }
    
    /* NULL terminate buffer. */
    randomBuffer[5999] = '\0';
    
    /* Hash the 5999 bytes from /dev/urandom with sha512. */
    rtrn = sha512(randomBuffer, &hash);
    if(rtrn < 0)
    {
        output(ERROR, "can't SHA512 buffer\n");
        return -1;
    }
    
    /* Add 64 bytes of prng state. */
    RAND_add(hash, 128, 64);
    
    /* We don't need this buffer anymore. */
    free(hash);
    
    /* Lets pick a number in the range of zero to 256 and
     we will use that as the number of loops to do. */
    rtrn = rand_range(256, (int *)&iterations);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }
    else if(iterations > 256)
    {
        output(ERROR, "Number returned is bigger than requested!\n");
        return -1;
    }
    
    /* Now we enter a loop, which adds more entrophy to the prng. */
    for(i = 0; i < iterations; i++)
    {
        /* Generate a random buffer length. */
        rtrn = rand_range(4096, (int *)&bufLen);
        if(rtrn < 0)
        {
            output(ERROR, "Can't generate random number\n");
            return -1;
        }
        else if(iterations > 4096)
        {
            output(ERROR, "Number returned is bigger than requested!\n");
            return -1;
        }

        /* Create a random buffer with the random buf length. */
        randomBuffer = realloc(randomBuffer, (size_t)bufLen + 1);
        if(randomBuffer == NULL)
        {
            output(ERROR, "realloc: $s\n", strerror(errno));
            return -1;
        }
        
        /* Read from /dev/urandom. */
        ret = read(randomFd, randomBuffer, (size_t)bufLen);
        if(ret != bufLen)
        {
            output(ERROR, "read: %s\n", strerror(errno));
            return -1;
        }
        
        /* NULL terminate the randomBuffer. */
        randomBuffer[bufLen] = '\0';
        
        /* Hash the randomBuffer. */
        rtrn = sha512(randomBuffer, &hash);
        if(rtrn < 0)
        {
            output(ERROR, "can't SHA512 buffer\n");
            return -1;
        }
        
        /* Add 64 bytes of prng state. */
        RAND_add(hash, 128, 64);
        
        /* Clean up hash buffer. */
        free(hash);
        
    }
    
    /* Clean up. */
    free(randomBuffer);
    close(randomFd);
    
    return 0;
}

int setup_crypto(void)
{
    output(STD, "Setting up crypto\n");

    int rtrn;

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
            case 0: output(STD, "Using hardware crypto accelerator\n"); break;

            case 1: output(STD, "Switching to /dev/urandom \n"); return seed_prng();

            default: output(ERROR, "Error while trying to setup hardware acceleration\n"); return -1;
        }  
    }
    
    return 0;
}

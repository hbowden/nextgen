/**
 * Copyright (c) 2015, Harrison Bowden, Minneapolis, MN
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
#include "io/io.h"
#include "memory/memory.h"
#include "openssl/bn.h"
#include "openssl/crypto.h"
#include "openssl/engine.h"
#include "openssl/evp.h"
#include "openssl/rand.h"
#include "openssl/sha.h"
#include "platform.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static int32_t (*rand_range_pointer)(uint32_t range, uint32_t *number);

static int32_t software_prng;

static int32_t crypto_setup;

int32_t using_hardware_prng(void) { return (software_prng); }

static int32_t rand_range_no_crypto(uint32_t range, uint32_t *number)
{
    *number = (uint32_t)rand() % range + 1;
    return (0);
}

int32_t rand_bytes(char **buf, uint32_t length)
{
    int32_t rtrn = RAND_bytes((unsigned char *)*buf, (int32_t)length);
    if(rtrn != 1)
    {
        output(STD, "Can't get random bytes\n");
        return (-1);
    }

    /* null terminate the string. */
    (*buf)[length] = '\0';

    return (0);
}

static int32_t rand_range_crypto(uint32_t range, uint32_t *number)
{
    int32_t rtrn = 0;
    BIGNUM *random, *range1;

    random = BN_new();
    if(random == NULL)
    {
        output(ERROR, "Can't init bignum struct\n");
        return (-1);
    }

    range1 = BN_new();
    if(random == NULL)
    {
        output(ERROR, "Can't init bignum struct\n");
        return (-1);
    }

    rtrn = BN_set_word(range1, (range + 1));
    if(rtrn < 0)
    {
        output(ERROR, "Can't set range\n");
        return (-1);
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
        return (-1);
    }

    (*number) = (uint32_t)strtol(buf, NULL, 10);

    return (0);
}

static int32_t setup_rand_range(enum crypto_method method)
{
    if(method == NO_CRYPTO)
    {
        rand_range_pointer = &rand_range_no_crypto;
    }
    else if(method == CRYPTO)
    {
        rand_range_pointer = &rand_range_crypto;
    }
    else
    {
        output(ERROR, "Bad argument to crypto option, try again\n");
        return (-1);
    }

    return (0);
}

int32_t rand_range(uint32_t range, uint32_t *number)
{
    /* Make sure the crypto module is set, so we don't
    crash when we call rand_range_pointer(). */
    if(crypto_setup != TRUE)
    {
        output(ERROR, "Call setup crypto first\n");
        return (-1);
    }

    /* Call the random range interface. */
    return (rand_range_pointer(range, number));
}

static int32_t setup_hardware_acceleration(void)
{
    ENGINE *engine = ENGINE_by_id("rdrand");
    if(engine == NULL)
    {
        output(STD, "No intel random number generator detected\n");
        return (1); /* Return one, when we can't detect a hardware random  */
    }

    /* Note. openssl use's zero for it's success exit code. */
    int32_t rtrn = ENGINE_init(engine);
    if(rtrn == 0)
    {
        output(ERROR, "Can't init crypto engine\n");
        return (-1);
    }

    rtrn = ENGINE_set_default(engine, ENGINE_METHOD_RAND);
    if(rtrn == 0)
    {
        output(ERROR, "Can't set default method\n");
        return (-1);
    }

    return (0);
}

int32_t sha512(char *in, char **out)
{
    SHA512_CTX ctx;
    uint32_t i = 0;
    int32_t rtrn = 0;
    unsigned char hash[SHA512_DIGEST_LENGTH];

    rtrn = SHA512_Init(&ctx);
    if(rtrn < 0)
    {
        output(ERROR, "Sha init\n");
        return (-1);
    }

    rtrn = SHA512_Update(&ctx, in, strlen(in));
    if(rtrn < 0)
    {
        output(ERROR, "Can't update input string\n");
        return (-1);
    }

    rtrn = SHA512_Final(hash, &ctx);
    if(rtrn < 0)
    {
        output(ERROR, "Can't do this\n");
        return (-1);
    }

    (*out) = mem_alloc((SHA512_DIGEST_LENGTH * 2) + 1);
    if((*out) == NULL)
    {
        output(ERROR, "Can't allocate output buf\n");
        return (-1);
    }

    for(i = 0; i < SHA512_DIGEST_LENGTH; i++)
        sprintf(*out + (i * 2), "%02x", hash[i]);

    (*out)[128] = '\0';

    return (0);
}

int32_t sha256(char *in, char **out)
{
    /* Declarations */
    uint32_t i = 0;
    int32_t rtrn = 0;
    SHA256_CTX sha256;
    unsigned char hash[SHA256_DIGEST_LENGTH];

    rtrn = SHA256_Init(&sha256);
    if(rtrn < 0)
    {
        output(ERROR, "Sha Init Error\n");
        return (-1);
    }

    rtrn = SHA256_Update(&sha256, in, strlen(in));
    if(rtrn < 0)
    {
        output(ERROR, "Sha Update Error\n");
        return (-1);
    }

    rtrn = SHA256_Final(hash, &sha256);
    if(rtrn < 0)
    {
        output(ERROR, "Sha Final Error\n");
        return (-1);
    }

    /* Allocate the output buffer. */
    (*out) = mem_alloc((SHA256_DIGEST_LENGTH * 2) + 1);
    if((*out) == NULL)
    {
        output(ERROR, "Can't allocate output buf\n");
        return (-1);
    }

    /* Convert hash to hex format. */
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(*out + (i * 2), "%02x", hash[i]);

    /* null terminate the hash. */
    (*out)[64] = '\0';

    return (0);
}

/*  */
int32_t seed_prng(void)
{
    /* The variables used in seed_prng(). */
    int32_t rtrn = 0;
    ssize_t ret;
    char *hash auto_free = NULL;
    int32_t random_fd auto_close = 0;
    uint32_t i, loops, buf_len;
    char *random_buffer auto_free = NULL;

    /* Allocate a buffer thats 6000 bytes long, this is
      where we will put random stuff from /dev/urandom. */
    random_buffer = mem_alloc(6000);
    if(random_buffer == NULL)
    {
        output(ERROR, "Can't allocate random buffer\n");
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
    ret = read(random_fd, random_buffer, 5999);
    if(ret != 5999)
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

int32_t setup_crypto_module(enum crypto_method method)
{
    output(STD, "Setting up crypto\n");

    /* Check if we have been setup already. */
    if(crypto_setup == TRUE)
    {
        output(ERROR, "Crypto already setup\n");
        return (-1);
    }

    /* Good ol rtrn. */
    int32_t rtrn = 0;

    /* Init openssl/libressl library. */
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();

    /* Pass the crypto method the user want's to setup_rand_range(). */
    rtrn = setup_rand_range(method);
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup random range function.\n");
        return (-1);
    }

    /* If the user does not wan't cryptographic numbers exit early. */
    if(method == NO_CRYPTO)
    {
        /* Set the crypto_setup flag so we know we have been setup. */
        crypto_setup = TRUE;
        return (0);
    }

    /* Check for hardware crypto accelators and use them if present, otherwise use
    /dev/urandom .*/
    rtrn = setup_hardware_acceleration();
    switch(rtrn)
    {
        case 0:
            output(STD, "Using hardware crypto PRNG\n");
            software_prng = 0;
            crypto_setup = TRUE;
            break;

        case 1:
            output(STD, "Switching to /dev/urandom \n");
            output(STD, "Seeding PRNG\n");
            software_prng = 1;
            crypto_setup = TRUE;
            return (seed_prng());

        default:
            output(ERROR,
                   "Error while trying to setup hardware acceleration\n");
            return (-1);
    }

    return (0);
}

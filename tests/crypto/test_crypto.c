

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

#include "test_utils.h"
#include "../../src/crypto.c"
#include "memory.h"
#include "io.h"

//static unsigned int range = 1000;
static uint32_t iterations = 1000;

static int test_sha256(void)
{
	log_test(DECLARE, "Testing sha256");

	uint32_t i = 0;
    int32_t rtrn = 0;
    char *buf = NULL;
    char *hash = NULL;

    /* Loop and test sha256 several times. */
	for(i = 0; i < iterations; i++)
	{
        /* Allocate a buffer to place random bytes in. */
        buf = mem_alloc(1001);
        if(buf == NULL)
        {
            output(ERROR, "Can't allocate buffer\n");
            return -1;
        }

		/* Create a random buffer with rand_bytes. */
        rtrn = rand_bytes(&buf, 1000);
        if(rtrn < 0)
        {
            output(ERROR, "Can't get random bytes\n");
            return -1;
        }

        /* buf should not be NULL. */
        assert_stat(buf != NULL);

        /* sha256 hash the random buffer. */
        rtrn = sha256(buf, &hash);

        /* sha256 should return 0 on success. */
        assert_stat(rtrn == 0);

        /* The hash should not be NULL. */
        assert_stat(hash != NULL);

        /* The hash should be 64 bytes long. */
        assert_stat(strlen(hash) == 64);

        /* Clean up the buffers. */
        mem_free(hash);
        mem_free(buf);
	}

	log_test(SUCCESS, "sha256 test passed");

	return 0;
}

static int32_t test_sha512(void)
{
	log_test(DECLARE, "Testing sha512");

	uint32_t i = 0;
    int32_t rtrn = 0;
    char *buf = NULL;
    char *hash = NULL;

    /* Loop and test sha512 several times. */
	for(i = 0; i < iterations; i++)
	{
        /* Allocate a buffer to place random bytes in. */
        buf = mem_alloc(1001);
        if(buf == NULL)
        {
            output(ERROR, "Can't allocate buffer\n");
            return (-1);
        }

		/* Create a random buffer with rand_bytes. */
        rtrn = rand_bytes(&buf, 1000);
        if(rtrn < 0)
        {
            output(ERROR, "Can't get random bytes\n");
            return (-1);
        }

        /* buf should not be NULL. */
        assert_stat(buf != NULL);

        /* sha512 hash the random buffer. */
        rtrn = sha512(buf, &hash);

        /* sha512 should return 0 on success. */
        assert_stat(rtrn == 0);

        /* The hash should not be NULL. */
        assert_stat(hash != NULL);

        /* The hash should be 128 bytes long. */
        assert_stat(strlen(hash) == 128);

        /* Clean up the buffers. */
        mem_free(hash);
        mem_free(buf);
	}

	log_test(SUCCESS, "sha512 test passed");
	return 0;
}

static int32_t test_setup_crypto(void)
{
    log_test(DECLARE, "Testing crypto module setup");

    int32_t rtrn = 0;

    rtrn = setup_crypto_module(CRYPTO);
    assert_stat(rtrn == 0);

    assert_stat(crypto_setup == TRUE);

    assert_stat(rand_range_pointer != NULL);

    log_test(SUCCESS, "crypto module setup test passed");

	return (0);
}

static int32_t test_rand_range(void)
{
    log_test(DECLARE, "Testing random range");

    int32_t rtrn = 0;
    uint32_t number = 0;
    uint32_t i = 0;

    for(i = 0; i < iterations; i++)
    {
        rtrn = rand_range(1000, &number);

        assert_stat(rtrn == 0);

        assert_stat(number <= 1000);
    }

    log_test(SUCCESS, "random range test passed");

	return (0);
}

static int32_t test_rand_bytes(void)
{
    log_test(DECLARE, "Testing random bytes");

    int32_t rtrn = 0;
    char *buf = NULL;
    uint32_t i = 0;

    for(i = 0; i < iterations; i++)
    {
        buf = mem_alloc(1001);
        if(buf == NULL)
        {
            output(ERROR, "Can't allocate buf\n");
            return (-1);
        }

        rtrn = rand_bytes(&buf, 1000);

        assert_stat(rtrn == 0);

        assert_stat(strlen(buf) <= 1000);

        mem_free(buf);
    }

    log_test(SUCCESS, "random bytes test passed");

	return (0);
}

int main(void)
{
    int32_t rtrn = 0;

    rtrn = init_test_framework();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init test framework");
        return (-1);
    }

    test_stat = init_stats_obj();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't init the stats object\n");
        return (-1);
    }

    rtrn = test_setup_crypto();
    if(rtrn < 0)
        log_test(FAIL, "setup crypto test failed");

    rtrn = test_sha256();
    if(rtrn < 0)
        log_test(FAIL, "sha256 test failed");

    rtrn = test_sha512();
    if(rtrn < 0)
        log_test(FAIL, "sha512 test failed");

    rtrn = test_rand_range();
    if(rtrn < 0)
        log_test(FAIL, "random range test failed");

    rtrn = test_rand_bytes();
    if(rtrn < 0)
        log_test(FAIL, "random bytes test failed");

    return (0);
}

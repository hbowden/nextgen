

/**
 * Copyright (c) 2015, Harrison Bowden, Minneapolis, MN
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice 
 * and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY å
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **/

#include "test_utils.h"
#include "../../src/io.h"
#include "../../src/utils.c"
#include "../../src/crypto.h"
#include "../../src/memory.h"

#include <stdint.h>
#include <unistd.h>

/* The number of ascii to binary comparison test. */
static uint32_t binary_test = 6;

/* Array of strings to covert to binary. */
static char *string_array[] = { "a", "b", "c", "d", "e", "f", NULL };

/* Array of binary strings to compare against. */
static char *binary_array[] = { "01100001", "01100010", "01100011", "01100100",
                                "01100101", "01100110", NULL };

static int32_t test_check_root(void)
{
	log_test(DECLARE, "Testing check root");

	int32_t rtrn = 0;
    uid_t uid = getuid();

    rtrn = check_root();

    if(uid != 0)
    {
        assert_stat(rtrn != 0);
    }
    else
    {
        assert_stat(rtrn == 0);
    }

    log_test(SUCCESS, "Check root test passed");

	return (0);
}

static int32_t test_get_file_size(void)
{
    log_test(DECLARE, "Testing get file size");

    uint32_t i;
    int32_t rtrn = 0;

    for(i = 0; i < 100; i++)
    {
        char *name = NULL;
        char *junk = NULL;
        char *path = NULL;
        uint32_t size = 0;

        rtrn = generate_name(&name, ".txt", FILE_NAME);
        assert_stat(rtrn == 0);
        assert_stat(name != NULL);

        rtrn = asprintf(&path, "/tmp/%s", name);
        if(rtrn < 0)
        {
            output(ERROR, "Can't join paths: %s\n", strerror(errno));
            return -1;
        }

        rtrn = rand_range(4096, &size);
        if(rtrn < 0)
        {
            output(ERROR, "Can't choose randon number\n");
            return -1;
        }

        /* Allocate a buffer to put junk in. */
        junk = mem_alloc(size + 1);
        if(junk == NULL)
        {
            output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
            return -1;
        }

        /* Put some junk in a buffer. */
        rtrn = rand_bytes(&junk, size);
        if(rtrn < 0)
        {
            output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
            return -1;
        }

        /* Write that junk to the file so that the file is not just blank. */
        rtrn = map_file_out(path, junk, size);
        if(rtrn < 0)
        {
            output(ERROR, "Can't write junk to disk\n");
            return -1;
        }

        int32_t fd = open(path, O_RDONLY);
        if(fd < 0)
        {
            output(ERROR, "Can't open file: %s\n", strerror(errno));
            return (-1);
        }

        uint64_t file_size = 0;

        rtrn = get_file_size(fd, &file_size);
        assert_stat(rtrn == 0);
        assert_stat(size == (uint32_t)file_size);

        /* Clean up. */
        unlink(path);
        mem_free(name);
        mem_free(junk);
        mem_free(path);
    }

    log_test(SUCCESS, "Get file size test passed");

    return (0);
}

static int32_t test_ascii_to_binary(void)
{
    log_test(DECLARE, "Testing ascii to binary");

    uint32_t i;
    int32_t rtrn = 0;

    char *string = "dn39r93ho*(#98893rnf@N(EHM(EI";
    char *binary_string = NULL;

    rtrn = ascii_to_binary(string, &binary_string, strlen(string));
    assert_stat(rtrn == 0);
    assert_stat(binary_string != NULL);

    /* Clean up. */
    mem_free(binary_string);

    /* Loop and compare ascii to binary conversions
    to validate ascii_to_binary(); */
    for(i = 0; i < binary_test; i++)
    {
        /* Convert the string in  */
        rtrn = ascii_to_binary(string_array[i], &binary_string, strlen(string_array[i]));
        assert_stat(rtrn == 0);
        assert_stat(binary_string != NULL);
        assert_stat(strncmp(binary_array[i], binary_string, 1) == 0);
        mem_free(binary_string);
    }
    
    log_test(SUCCESS, "ascii to binary test passed");

    return (0);
}

int main(void)
{
	int32_t rtrn = 0;

    /* Init the unit testing framework. */
    rtrn = init_test_framework();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init test framework");
        return (-1);
    }

    /* Initialize the stats object. */
    test_stat = init_stats_obj();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't init the stats object\n");
        return (-1);
    }

    /* The crypto module most be setup first before
    using the crypto module. */
    rtrn = setup_crypto_module(CRYPTO);
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup crypto module");
        return (-1);
    }

    /* Most */
    rtrn = test_check_root();
    if(rtrn < 0)
    	log_test(FAIL, "Check root test failed");

    rtrn = test_get_file_size();
    if(rtrn < 0)
        log_test(FAIL, "Get file size test failed");

    rtrn = test_ascii_to_binary();
    if(rtrn < 0)
        log_test(FAIL, "Ascii to binary test failed");

    _exit(0);
}

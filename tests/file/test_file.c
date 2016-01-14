

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
#include "../../src/file.c"
#include "../../src/memory.h"
#include "../../src/crypto.h"
#include "../../src/utils.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

static uint32_t loops;

static int32_t test_count_files_directory(void)
{
    log_test(DECLARE, "Testing count directory");

    int32_t rtrn = 0;
    uint32_t count = 0;

    rtrn = count_files_directory(&count);

    /* rtrn should equal file module. */
    assert_stat(rtrn == 0);
    assert_stat(count > 0);
    assert_stat(loops == count);

    log_test(SUCCESS, "count directory test passed");

    return (0);
}

static int32_t test_setup_file_module(void)
{
    log_test(DECLARE, "Testing file module setup");

    int32_t rtrn = 0;

    char *tmp_dir = NULL;

    rtrn = setup_crypto_module(CRYPTO);
    assert_stat(rtrn == 0);

    char *dir = NULL;

    rtrn = generate_name(&dir, NULL, DIR_NAME);
    assert_stat(rtrn == 0);

    rtrn = asprintf(&tmp_dir, "/tmp/%s", dir);
    if(rtrn < 0)
    {
        perror("asprintf");
        return (-1);
    }

    /* Create a temporary directory for testing. */
    rtrn = mkdir(tmp_dir, 0777);
    if(rtrn < 0)
    {
        perror("mkdir");
        return (-1);
    }

    /* Loop and create a random amount of tmp files. */
    rtrn = rand_range(100, &loops);
    assert_stat(rtrn == 0);

    uint32_t i;

    for(i = 0; i < loops; i++)
    {
        char *name = NULL;
        char *junk = NULL;
        char *path = NULL;

        rtrn = generate_name(&name, ".txt", FILE_NAME);
        assert_stat(rtrn == 0);
        assert_stat(name != NULL);

        rtrn = asprintf(&path, "/tmp/%s/%s", dir, name);
        if(rtrn < 0)
        {
            output(ERROR, "Can't join paths: %s\n", strerror(errno));
            return -1;
        }

        /* Allocate a buffer to put junk in. */
        junk = mem_alloc(4096);
        if(junk == NULL)
        {
            output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
            return -1;
        }

        /* Put some junk in a buffer. */
        rtrn = rand_bytes(&junk, 4095);
        if(rtrn < 0)
        {
            output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
            return -1;
        }

        /* Write that junk to the file so that the file is not just blank. */
        rtrn = map_file_out(path, junk, 4095);
        if(rtrn < 0)
        {
            output(ERROR, "Can't write junk to disk\n");
            return -1;
        }

        /* Clean up. */
        mem_free(name);
        mem_free(junk);
        mem_free(path);
    }

    /* Give the file module the path of the program to test
      and the directory of input files. */
    rtrn = setup_file_module("/bin/ls", tmp_dir);

    /* rtrn should equal file module. */
    assert_stat(rtrn == 0);

    log_test(SUCCESS, "Setup file module test passed");

	return (0);
}

static int32_t test_create_file_index(void)
{
    log_test(DECLARE, "Testing create file index");

    /* Just check the file array because create_file_index() gets called in
    setup_file_module(). */
    uint32_t i;

    for(i = 0; i < file_count; i++)
    {
        int32_t fd = 0;

        assert_stat(file_index[i] != NULL);
        fd = open(file_index[i], O_RDONLY);
        assert_stat(fd > 0);
        assert_stat(close(fd) == 0);
    }
    
    log_test(SUCCESS, "Create file index test passed");

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

    /* Initialize the stats object. */
    test_stat = init_stats_obj();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't init the stats object\n");
        return (-1);
    }

    rtrn = test_setup_file_module();
    if(rtrn < 0)
    	log_test(FAIL, "Setup file module test failed");

    rtrn = test_count_files_directory();
    if(rtrn < 0)
        log_test(FAIL, "Count files directory test failed");

    rtrn = test_create_file_index();
    if(rtrn < 0)
        log_test(FAIL, "Create file index test failed");

	_exit(0);
}

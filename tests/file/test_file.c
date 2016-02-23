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
#include "file/file.c"
#include "memory/memory.h"
#include "crypto/crypto.h"
#include "utils/utils.h"
#include "stdatomic.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

static uint32_t loops;
static uint32_t iterations = 10000;
static char *tmp_dir = NULL;

static int32_t test_get_file(void)
{
    log_test(DECLARE, "Testing get file");

    uint32_t i;
    int32_t rtrn = 0;
    int32_t file = 0;
    char *extension = NULL;

    for(i = 0; i < iterations; i++)
    {
        rtrn = get_file(&file, &extension);
        assert_stat(rtrn == 0);
        assert_stat(file > 0);
        assert_stat(write(file, "123456789", 9) == 9);
        assert_stat(extension != NULL);
    }

    log_test(SUCCESS, "Get file test passed");

    return (0);
}

static int32_t test_init_file_array(void)
{
    log_test(DECLARE, "Testing init file array");

    uint32_t i;
    int32_t rtrn = 0;
    struct file_ctx **array = NULL;

    array = init_file_array(tmp_dir, file_count);
    assert_stat(array != NULL);

    for(i = 0; i < file_count; i++)
    {
        assert_stat(&array[i] != NULL);
        assert_stat(array[i]->path != NULL);
        assert_stat(array[i]->type == BINARY || array[i]->type == TEXT);
        assert_stat(array[i]->extension != NULL);
    }

    log_test(SUCCESS, "Init file array test passed");

    return (0);
}

/*static int32_t test_detect_file_type(void)
{
    log_test(DECLARE, "Testing detect file type");

    int32_t rtrn = 0;

    uint32_t i;

    for(i = 0; i < file_count; i++)
    {
        enum file_type type;

        rtrn = detect_file_type(file_array[i]->path, &type);
        assert_stat(rtrn == 0);
        assert_stat(type == BINARY || type == TEXT);
    }

    log_test(SUCCESS, "Detect file type test passed");

    return (0);
} */

static int32_t test_count_files_directory(void)
{
    log_test(DECLARE, "Testing count directory");

    int32_t rtrn = 0;
    uint32_t count = 0;

    rtrn = count_files_directory(&count, tmp_dir);

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
    char *dir = NULL;

    /* Create a random directory name. */
    rtrn = generate_name(&dir, NULL, DIR_NAME);
    assert_stat(rtrn == 0);

    /* Create the temp directory path string. */
    rtrn = asprintf(&tmp_dir, "/tmp/%s", dir);
    assert_stat(rtrn > 0);

    /* Create a temporary directory for testing. */
    rtrn = mkdir(tmp_dir, 0777);
    if(rtrn < 0)
    {
        perror("mkdir");
        return (-1);
    }

    rtrn = rand_range(100, &loops);
    assert_stat(rtrn == 0);

    uint32_t i;

    for(i = 0; i < loops; i++)
    {
        char *path = NULL;
        uint64_t size = 0;

        rtrn = create_random_file(tmp_dir, ".txt", &path, &size);
        assert_stat(rtrn == 0);
        assert_stat(path != NULL);
        assert_stat(size > 0);
    }

    atomic_int_fast32_t stop;

    atomic_init(&stop, FALSE);

    /* Give the file module the path of the program to test
      and the directory of input files. */
    rtrn = setup_file_module(&stop, "/bin/ls", tmp_dir);
    assert_stat(rtrn == 0);

    for(i = 0; i < file_count; i++)
    {
        int32_t fd = 0;

        assert_stat(file_array != NULL);
        assert_stat(file_array[i] != NULL);
        fd = open(file_array[i]->path, O_RDONLY);
        assert_stat(fd > 0);
        assert_stat(close(fd) == 0);
    }

    log_test(SUCCESS, "Setup file module test passed");

	return (0);
}

int main(void)
{
	int32_t rtrn = 0;

    test_stat = init_test_framework();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't init test framework");
        return (-1);
    }

    rtrn = setup_crypto_module(CRYPTO);
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup crypto module\n");
        return (-1);
    }

    rtrn = test_setup_file_module();
    if(rtrn < 0)
    	log_test(FAIL, "Setup file module test failed");

  /*  rtrn = test_detect_file_type();
    if(rtrn < 0)
        log_test(FAIL, "Detect file type test failed"); */

    rtrn = test_init_file_array();
    if(rtrn < 0)
        log_test(FAIL, "Init file array test failed");

    rtrn = test_count_files_directory();
    if(rtrn < 0)
        log_test(FAIL, "Count files directory test failed");

    rtrn = test_get_file();
    if(rtrn < 0)
        log_test(FAIL, "Get file test failed");

	_exit(0);
}

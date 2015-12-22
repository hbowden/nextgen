

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

static int32_t test_count_files_directory(void)
{
    log_test(DECLARE, "Testing count directory");

    int32_t rtrn = 0;
    uint32_t count = 0;

    rtrn = count_files_directory(&count);

    /* rtrn should equal file module. */
    assert_stat(rtrn == 0);
    assert_stat(count > 0);

    log_test(SUCCESS, "count directory test passed");

    return (0);
}

static int32_t test_setup_file_module(void)
{
    log_test(DECLARE, "Testing file module setup");

    int32_t rtrn = 0;

    char *tmp_dir = NULL;

    rtrn = asprintf(&tmp_dir, "/tmp/tmp_dir");
    if(rtrn < 0)
    {
        perror("asprintf");
        return (-1);
    }

    /* Give the file module the path of the program to test. */
    rtrn = setup_file_module("/bin/ls");

    /* rtrn should equal file module. */
    assert_stat(rtrn == 0);

    log_test(SUCCESS, "Setup file module test passed");

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
    stat = init_stats_obj();
    if(stat == NULL)
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

	return (0);
}



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

#include "../src/utils.h"
#include "nextgen.h"
#include "memory.h"
#include "test_utils.h"
#include "io.h"

#include <stdio.h>
#include <assert.h>

static int test_cmd_parser(void)
{
	log_test(DECLARE, "Testing command line parser");

	int argc = 1;
	int rtrn;
	char **args;
	struct parser_ctx *ctx = NULL;
    unsigned int i;

    args = mem_alloc(sizeof(char *) * 6);
    if(args == NULL)
    	return -1;

    for(i = 0; i < 6; i++)
    {
    	args[i] = mem_alloc(1025);
    	if(args[i] == NULL)
    		return -1;
    }

    /* Make sure a null ctx struct is returned because we didn't
    specify a fuzzing mode. */
	ctx = parse_cmd_line(argc, args);
	assert_stat(ctx == NULL);

	/* If argc is zero return a NULL parser context struct. */
	argc = 0;
	args[1] = "--syscall"; /* First arg */

	ctx = parse_cmd_line(argc, args);
	assert_stat(ctx == NULL);

    char *home = NULL;
    char *arg_string = NULL;

	rtrn = get_home(&home);
	if(rtrn < 0)
		return -1;

	rtrn = asprintf(&arg_string, "%s/syscall_results", home);
	if(rtrn < 0)
		return -1;

	/* Make sure syscall mode works. */
	argc = 3;
	args[0] = "nextgen"; /* Program name */
	args[1] = "--syscall"; /* First arg */
	args[2] = "--out"; /* Second arg */
	args[3] = arg_string;

	ctx = parse_cmd_line(argc, args);
	assert_stat(ctx != NULL);
    assert_stat(ctx->mode == MODE_SYSCALL);
    assert_stat(ctx->path_to_out_dir != NULL);
    assert_stat(strncmp(ctx->path_to_out_dir, arg_string, strlen(arg_string)) == 0);

    log_test(SUCCESS, "Passed command line parser test");

    return 0;
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

	rtrn = test_cmd_parser();
	if(rtrn < 0)
    {
        log_test(FAIL, "Command line parser test failed");
	    return -1;
    }

	return 0;
}



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

#include "log.h"
#include "arg_types.h"
#include "utils.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

static char *decode_arg_type(enum arg_types arg)
{
	int rtrn;
	char *arg_string = NULL;

    switch((int)arg)
    {
    	case FILE_DESC:
    	    rtrn = asprintf(&arg_string, "%s", "FILE_DESC");
    	    if(rtrn < 0)
    	    {
    	    	output(ERROR, "Can't create arg string: %s\n", strerror(errno));
    	    	return NULL;
    	    }
    	    break;

    	case VOID_BUFF:
    	    rtrn = asprintf(&arg_string, "%s", "VOID_BUFF");
    	    if(rtrn < 0)
    	    {
    	    	output(ERROR, "Can't create arg string: %s\n", strerror(errno));
    	    	return NULL;
    	    }
    	    break;

    	case SIZE_T:
    	    rtrn = asprintf(&arg_string, "%s", "SIZE_T");
    	    if(rtrn < 0)
    	    {
    	    	output(ERROR, "Can't create arg string: %s\n", strerror(errno));
    	    	return NULL;
    	    }
    	    break;

    	case FILE_PATH:
    	    rtrn = asprintf(&arg_string, "%s", "FILE_PATH");
    	    if(rtrn < 0)
    	    {
    	    	output(ERROR, "Can't create arg string: %s\n", strerror(errno));
    	    	return NULL;
    	    }
    	    break;

    	case OPEN_FLAG:
    	    rtrn = asprintf(&arg_string, "%s", "OPEN_FLAG");
    	    if(rtrn < 0)
    	    {
    	    	output(ERROR, "Can't create arg string: %s\n", strerror(errno));
    	    	return NULL;
    	    }
    	    break;

    	case MODE_T:
    	    rtrn = asprintf(&arg_string, "%s", "MODE_T");
    	    if(rtrn < 0)
    	    {
    	    	output(ERROR, "Can't create arg string: %s\n", strerror(errno));
    	    	return NULL;
    	    }
    	    break;

    	case STAT_FS:
            rtrn = asprintf(&arg_string, "%s", "STAT_FS");
    	    if(rtrn < 0)
    	    {
    	    	output(ERROR, "Can't create arg string: %s\n", strerror(errno));
    	    	return NULL;
    	    }
    	    break;

    	case STAT_FLAG:
    	    rtrn = asprintf(&arg_string, "%s", "STAT_FLAG");
    	    if(rtrn < 0)
    	    {
    	    	output(ERROR, "Can't create arg string: %s\n", strerror(errno));
    	    	return NULL;
    	    }
    	    break;

    	default:
    	    output(ERROR, "Unknown arg type.\n");
    	    return NULL;
    }

	return arg_string;
}

int create_out_directory(char *path)
{

	return 0;
}

int log_arguments(struct child_ctx *ctx)
{
    int rtrn;
    unsigned int i;
    unsigned int number_of_args = map->sys_table->sys_entry[ctx->syscall_number].number_of_args;
    const char *name_of_syscall = map->sys_table->sys_entry[ctx->syscall_number].name_of_syscall;

    char *out_buf auto_clean_buf = malloc(4096);
    if(out_buf == NULL)
    {
       output(ERROR, "Can't create out buffer: %s\n", strerror(errno));
       return -1;
    }

    for(i = 0; i < number_of_args; i++)
    {
    	char *arg_value;

        strlcat(out_buf, decode_arg_type(map->sys_table->sys_entry[ctx->syscall_number].arg_type_index[i]), 4095);

    	asprintf(&arg_value, "%lu ", ctx->arg_value_index[i]);

    	strlcat(out_buf, arg_value, 4095);

    	free(arg_value);
    }

    out_buf[4095] = '\0';

    char *syscall_log_buf auto_clean_buf = NULL;
    
    rtrn = asprintf(&syscall_log_buf, "%s: %s", name_of_syscall, out_buf);
    if(syscall_log_buf == NULL)
    {
    	output(ERROR, "Can't create syscall log buffer: %s\n", strerror(errno));
    	return -1;
    }

    output(STD, "%s", syscall_log_buf);

	return 0;
}

int log_results(struct child_ctx *ctx)
{

	return 0;
}
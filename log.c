

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

    	case INT:
    	    rtrn = asprintf(&arg_string, "%s", "INT");
    	    if(rtrn < 0)
    	    {
    	    	output(ERROR, "Can't create arg string: %s\n", strerror(errno));
    	    	return NULL;
    	    }
    	    break;

    	case RUSAGE:
    	    rtrn = asprintf(&arg_string, "%s", "RUSAGE");
    	    if(rtrn < 0)
    	    {
    	    	output(ERROR, "Can't create arg string: %s\n", strerror(errno));
    	    	return NULL;
    	    }
    	    break;

        case PID:
            rtrn = asprintf(&arg_string, "%s", "PID");
    	    if(rtrn < 0)
    	    {
    	    	output(ERROR, "Can't create arg string: %s\n", strerror(errno));
    	    	return NULL;
    	    }
    	    break;

        case WAIT_OPTION:
            rtrn = asprintf(&arg_string, "%s", "WAIT_OPTION");
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

    char *arg_value auto_clean_buf = malloc(1024);
    if(arg_value == NULL)
    {
       output(ERROR, "Can't create arg_value buffer: %s\n", strerror(errno));
       return -1;
    }

    char *syscall_log_buf auto_clean_buf = malloc(4096);
     if(syscall_log_buf == NULL)
    {
       output(ERROR, "Can't create syscall_log_buf buffer: %s\n", strerror(errno));
       return -1;
    }

    sprintf(syscall_log_buf, "%s:", name_of_syscall);

    for(i = 0; i < number_of_args; i++)
    {
    	switch((int)map->sys_table->sys_entry[ctx->syscall_number].arg_type_index[i])
    	{

    		/* File and directory paths. */
    		case FILE_PATH:
    		    sprintf(arg_value, " %s=%s", decode_arg_type(map->sys_table->sys_entry[ctx->syscall_number].arg_type_index[i]), (char *)ctx->arg_value_index[i]);
    		    break;

            /* Pointers. */
    		case RUSAGE:
    		case VOID_BUFF:
    		case STAT_FS:
    		    sprintf(arg_value, " %s=%p", decode_arg_type(map->sys_table->sys_entry[ctx->syscall_number].arg_type_index[i]), (void *)ctx->arg_value_index[i]);
    		    break;

            /* Non pointer values. */
    		default:
    		    sprintf(arg_value, " %s=%lu", decode_arg_type(map->sys_table->sys_entry[ctx->syscall_number].arg_type_index[i]), *(ctx->arg_value_index[i]));
    		    break;
    	}

    	strncat(syscall_log_buf, arg_value, strlen(arg_value));
    }

    output(STD, "%s\n", syscall_log_buf);

	return 0;
}

int log_results(struct child_ctx *ctx)
{

	return 0;
}
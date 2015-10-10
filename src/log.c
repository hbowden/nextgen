

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
#include "io.h"
#include "memory.h"
#include "nextgen.h"
#include "arg_types.h"
#include "utils.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

int log_file(char *file_path, char *file_extension)
{
    int rtrn;
    char *out_path auto_clean = NULL;

    /* Create out file path. */
    rtrn = asprintf(&out_path, "%s/last_file_run.%s", map->path_to_out_dir, file_extension);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create out path: %s\n", strerror(errno));
        return -1;
    }

    /* Copy file to the out directory. */
    rtrn = copy_file_to(file_path, out_path);
    if(rtrn < 0)
    {
        output(ERROR, "Can't copy file to the out directory\n");
        return -1;
    }
    
    return 0;
}

int create_out_directory(char *path)
{
    int rtrn;
    char *crash_dir auto_clean = NULL;

    rtrn = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create out directory\n");

        /* Check if the directory exist. */
        if(errno == EEXIST)
        {
            output(STD, "Out directory already exist\n");
            return -1;
        }

        return -1;
    }

    rtrn = asprintf(&crash_dir, "%s/crash_dir", path);
    if(rtrn < 0)
    {
        output(STD, "Out directory already exist\n");
        return -1;
    }

    rtrn = mkdir(crash_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create crash directory\n");
        return -1;
    }

	return 0;
}

int log_arguments(struct child_ctx *ctx)
{
    unsigned int i;
    unsigned int number_of_args = map->sys_table->sys_entry[ctx->syscall_number].number_of_args;
    const char *name_of_syscall = map->sys_table->sys_entry[ctx->syscall_number].name_of_syscall;

    char *arg_value auto_clean = mem_alloc(1024);
    if(arg_value == NULL)
    {
       output(ERROR, "Can't create arg_value buffer\n");
       return -1;
    }

    char *syscall_log_buf auto_clean = mem_alloc(4096);
     if(syscall_log_buf == NULL)
    {
       output(ERROR, "Can't create syscall_log_buf buffer\n");
       return -1;
    }

    sprintf(syscall_log_buf, "%s:", name_of_syscall);

    for(i = 0; i < number_of_args; i++)
    {
    	switch((int)map->sys_table->sys_entry[ctx->syscall_number].arg_context_index[i]->log_type)
    	{
    		/* File and directory paths. */
    		case PATH:
    		    sprintf(arg_value, " %s=%s", map->sys_table->sys_entry[ctx->syscall_number].arg_context_index[i]->name, (char *)ctx->arg_value_index[i]);
    		    break;

            /* Pointers. */
    		case POINTER:
    		    sprintf(arg_value, " %s=%p", map->sys_table->sys_entry[ctx->syscall_number].arg_context_index[i]->name, (void *)ctx->arg_value_index[i]);
    		    break;

            /* Non pointer values. */
    		case NUMBER:
    		    sprintf(arg_value, " %s=%lu", map->sys_table->sys_entry[ctx->syscall_number].arg_context_index[i]->name, *(ctx->arg_value_index[i]));
    		    break;

            default:
                output(ERROR, "Unknown log type\n");
                return -1;
    	}

    	strncat(syscall_log_buf, arg_value, strlen(arg_value));
    }

    //output(STD, "%s\n", syscall_log_buf);

	return 0;
}

int log_results(struct child_ctx *ctx)
{
    int rtrn;
    char *out_buf auto_clean = NULL;

    if(ctx->had_error == YES)
    {
        rtrn = asprintf(&out_buf, "%s= %d (%s) %s\n", BOLD_RED, ctx->ret_value, ctx->err_value, RESET);
        if(rtrn < 0)
        {
            output(ERROR, "Can't create out message\n");
            return -1;
        }
    }
    else
    {
        rtrn = asprintf(&out_buf, "%s= %d %s\n", BOLD_GREEN, ctx->ret_value, RESET);
        if(rtrn < 0)
        {
            output(ERROR, "Can't create out message\n");
            return -1;
        }
    }

    output(STD, "%s", out_buf);

	return 0;
}

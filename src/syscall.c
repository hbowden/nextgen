

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

#include "syscall.h"
#include "entry.h"
#include "syscall_table.h"
#include "crypto.h"
#include "io.h"
#include "arg_types.h"
#include "utils.h"
#include "nextgen.h"
#include "shim.h"

#include <errno.h>
#include <string.h>

#include "stdatomic.h"

int cleanup_syscall_table(void)
{
    
    return 0;
}

static struct arg_context *get_arg_context(enum arg_type type)
{
    switch((int)type)
    {
        case FILE_DESC: return &file_desc_ctx;

        case VOID_BUF: return &void_buf_ctx;
            
        case SIZE: return &size_ctx;

        case FILE_PATH: return &file_path_ctx;

        case OPEN_FLAG: return &open_flag_ctx;
           
        case MODE: return &mode_ctx;

        case STAT_FS: return &stat_fs_ctx;

        case STAT_FLAG: return &stat_flag_ctx;

        case INT: return &int_ctx;
       
        case RUSAGE: return &rusage_ctx;
         
        case PID: return &pid_ctx;
          
        case WAIT_OPTION: return &wait_option_ctx;
         
        case SOCKET: return &socket_ctx;
           
        case WHENCE: return &whence_ctx;
           
        case OFFSET: return &offset_ctx;
        
        case MOUNT_TYPE: return &mount_type_ctx;
         
        case DIR_PATH: return &dir_path_ctx;
          
        case MOUNT_FLAG: return &mount_flag_ctx;

        case UNMOUNT_FLAG: return &unmount_flag_ctx;

        case RECV_FLAG: return &recv_flag_ctx;
         
        case REQUEST: return &request_ctx;

        case MOUNT_PATH: return &mount_path_ctx;

        case DEV: return &dev_ctx;

        default:
            output(ERROR, "Unlisted arg type\n");
            return NULL;
    }
}

int get_syscall_table(void)
{
    output(STD, "Building system call table\n");

    /* Grab a copy of the syscall table in on disk format. */ 
    struct syscall_table *sys_table = get_table();
    if(sys_table == NULL)
    {
        output(STD, "Can't grab syscall table\n");
        return -1;
    }

    /* Create a shadow syscall table.*/
    struct syscall_table_shadow *shadow_table = mem_alloc(sizeof(struct syscall_table_shadow));
    if(shadow_table == NULL)
    {
        output(ERROR, "Can't create shadow table\n");
        return -1;
    }

    /* Set the number_of_syscalls. */
    shadow_table->number_of_syscalls = sys_table->number_of_syscalls;

    /* Our loop incrementers. */
    unsigned int i, ii;

    /* Allocate heap memory for the list of syscalls. */
    shadow_table->sys_entry = mem_alloc(shadow_table->number_of_syscalls * sizeof(struct syscall_entry_shadow));
    if(shadow_table->sys_entry == NULL)
    {
        output(ERROR, "Can't create new entry\n");
        return -1;
    }

    /* This is the entry offset, it one because the entries start at [1] instead of [0]; */
    unsigned int offset = 1;

    /* We use this to track where to put the new entry in the entry index. */
    unsigned int shadow_entry_offset = 0;

    /* Loop for each entry syscall and build a table from the on disk format. */
    for(i = 0; i < shadow_table->number_of_syscalls; i++)
    {
    	/* Check if the syscall is OFF, this is usually for syscalls in development. */
    	if(sys_table[i + offset].sys_entry->status == OFF)
    	{
            shadow_table->number_of_syscalls--;
            continue;
    	}

        /* Create and intialize the const value for a shadow struct. */
        struct syscall_entry_shadow entry = {  

            .number_of_args = sys_table[i + offset].sys_entry->number_of_args, 
            .name_of_syscall = sys_table[i + offset].sys_entry->name_of_syscall 
        };

        /* Loop for each arg and set the arg index's. */
        for(ii = 0; ii < entry.number_of_args; ii++)
        {
            entry.get_arg_index[ii] = sys_table[i + offset].sys_entry->get_arg_index[ii];

            entry.arg_context_index[ii] = get_arg_context((enum arg_type)sys_table[i + offset].sys_entry->arg_type_index[ii]);
            if(entry.arg_context_index[ii] == NULL)
            {
                output(ERROR, "Can't gwt arg context\n");
                return -1;
            }
        }
     
        /* Init the automic value. */
        atomic_init(&entry.status, ON);

        /* Set the newly created entry in the index. */
        shadow_table->sys_entry[shadow_entry_offset] = entry;

        /* Increment offset. */
        shadow_entry_offset++;
    }

    map->sys_table = shadow_table;

    return 0;
}

/* This function is used to randomly pick the syscall to test. */
int pick_syscall(struct child_ctx *ctx)
{
    /* Use rand_range to pick a number between 0 and the number_of_syscalls.  */
    int rtrn = rand_range(map->sys_table->number_of_syscalls, &ctx->syscall_number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }

    /* Copy entry values into the child context. */
    ctx->number_of_args = map->sys_table->sys_entry[ctx->syscall_number].number_of_args;

    /* Set values in ctx so we can get info from ctx instead using all these pointers. */ 
    ctx->need_alarm = map->sys_table->sys_entry[ctx->syscall_number].need_alarm;

    ctx->syscall_symbol = map->sys_table->sys_entry[ctx->syscall_number].syscall_symbol;

    ctx->had_error = NO;

    return 0;
}

int generate_arguments(struct child_ctx *ctx)
{
    int rtrn;
    unsigned int i;
    unsigned int number_of_args = ctx->number_of_args;

    for(i = 0; i < number_of_args; i++)
    {
        ctx->current_arg = i;

        rtrn = map->sys_table->sys_entry[ctx->syscall_number].get_arg_index[i](&ctx->arg_value_index[i], ctx);
        if(rtrn < 0)
        {
            output(ERROR, "Can't generate arguments for: %s\n", map->sys_table->sys_entry[ctx->syscall_number].name_of_syscall);
            return -1;
        }
    }

	return 0;
}

static int check_for_failure(int ret_value)
{
    if(ret_value < 0)
        return -1;

    return 0;
}

int test_syscall(struct child_ctx *ctx)
{
    /* Grab argument values. */
    unsigned long *arg1 = ctx->arg_value_index[0];
    unsigned long *arg2 = ctx->arg_value_index[1];
    unsigned long *arg3 = ctx->arg_value_index[2];
    unsigned long *arg4 = ctx->arg_value_index[3];
    unsigned long *arg5 = ctx->arg_value_index[4];
    unsigned long *arg6 = ctx->arg_value_index[5];

    /* Check if we need to set the alarm for blocking syscalls.  */
    if(ctx->need_alarm == YES)
        alarm(1);

    /* Set the time of the syscall test. */
    (void)gettimeofday(&ctx->time_of_syscall, NULL);

#ifndef ASAN

    /* Do the add before the syscall test because we usually crash on the syscall test
    and we don't wan't to do this in a signal handler. */
    atomic_fetch_add(&map->test_counter, 1);

    /* Call the syscall with the args generated. */
    ctx->ret_value = syscall(ctx->syscall_symbol, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6);
    if(check_for_failure(ctx->ret_value) < 0)
    {
        /* If we got here, we had an error, so grab the error string. */
        ctx->err_value = strerror(errno);

        /* Set the error flag so the logging system knows we had an error. */
        ctx->had_error = YES;
    }
    
#endif

	return 0;
}

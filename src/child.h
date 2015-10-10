

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

#ifndef CHILD_H
#define CHILD_H

#include "private.h"
#include "utils.h"
#include "memory.h"

#include <setjmp.h>
#include <sys/time.h>
#include <unistd.h>

#include <ck_queue.h>

#include "stdatomic.h"

struct child_ctx
{
	/* The child's process PID. */
    atomic_int_fast32_t pid;

    /* A varible to store the address of where to jump back to in the child
    process on signals. */
    jmp_buf return_jump;

    /* This is the number used to identify and choose the syscall that's going to be tested. */
    unsigned int syscall_number;

    /* Symbolized syscall number. */
    int syscall_symbol;

    unsigned int current_arg;

    /* This index is where we store the arguments we generate. */
    unsigned long **arg_value_index;

    /* This index tracks the size of the arguments.*/
    unsigned long *arg_size_index;

    struct mem_pool_shared *pool;

    /* The child's resource cleanup list. */
    CK_LIST_HEAD(list, list_node) list;

    /* The number of args for the syscall were testing. */
    unsigned int number_of_args;

    bool need_alarm;

    /* Time that we made the syscall fuzz test. */
    struct timeval time_of_syscall;

    /* The return value of the last syscall called. */
    int ret_value;

    /* If this is set we know the syscall failed. */
    int had_error;

    /* The string value of errno if the syscall test failed. */
    char *err_value;

    /* This pipe is used to inform the main-loop process we are done
    creating and setting up the child process. */
    int msg_port[2];
};

private extern int get_child_index_number(void);

private extern void create_syscall_children(void);

private extern void create_smart_syscall_children(void);

private extern int test_exec_with_file_in_child(char *file_path, char *file_extension);

#endif

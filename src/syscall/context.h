

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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "stdatomic.h"
#include "concurrent/concurrent.h"

#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#include <dtrace.h>

struct child_ctx
{
    /* The child's process PID. */
    atomic_int_fast32_t pid;

    /* A varible to store the address of where to jump back to in the child
    process on signals. */
    jmp_buf return_jump;

    /* This is the number used to identify and choose the syscall that's going to be tested. */
    uint32_t syscall_number;

    /* Symbolized syscall number. */
    uint32_t syscall_symbol;

    /* */
    uint32_t current_arg;

    const char *name_of_syscall;

    /* This index is where we store the arguments we generate. */
    uint64_t **arg_value_index;

    uint64_t **arg_copy_index;

    /* This index tracks the size of the arguments.*/
    uint64_t *arg_size_index;

    struct mem_pool_shared *pool;

    /* The number of args for the syscall were testing. */
    uint32_t number_of_args;

    bool need_alarm;

    dtrace_hdl_t *probe_handle;

    /* Time that we made the syscall fuzz test. */
    struct timeval time_of_syscall;

    /* The return value of the last syscall called. */
    int32_t ret_value;

    /* If this is set we know the syscall failed. */
    int32_t had_error;

    /* The string value of errno if the syscall test failed. */
    char *err_value;

    /* This pipe is used to inform the main-loop process we are done
    creating and setting up the child process. */
    int32_t pipe_port[2];

    /* Child's message port. */
    msg_port_t msg_port;
};

#endif

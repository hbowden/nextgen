

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

#include "concurrent/concurrent.h"

#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

struct child_ctx
{
    /* The child's process PID. */
    int32_t pid;

    /* A varible to store the address of where to jump back to in the child
    process on signals. */
    jmp_buf return_jump;

    /* This is the number used to identify and choose the syscall that's going to be tested. */
    uint32_t syscall_number;

    /* Symbolized syscall number. */
    int32_t syscall_symbol;

    /* The argument were currently generating. */
    uint32_t current_arg;

    /* The number of args for the syscall were testing. */
    uint32_t total_args;

    /* The name of the syscall were testing. */
    const char *syscall_name;

    /* This array is where we store the arguments we generate. */
    uint64_t **arg_value_array;

    /* This array is a copy of the value array. */
    uint64_t **arg_copy_array;

    /* This index tracks the size of the arguments.*/
    uint64_t *arg_size_array;

    /* Time that we made the syscall fuzz test. */
    struct timeval time_of_syscall;

    /* The return value of the last syscall called. */
    int32_t ret_value;

    /* If this is set we know the syscall failed. */
    int32_t had_error;

    /* The string value of errno if the syscall test failed. */
    char *err_value;

    int32_t did_jump;

    struct probe_ctx *probe_handle;

    bool need_alarm;

    const char padding2[7];
};

#endif

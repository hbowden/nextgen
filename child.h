

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
#include "syscall_table.h"

#include <setjmp.h>
#include <stdatomic.h>
#include <sys/time.h>
#include <unistd.h>

struct child_ctx
{
	/* The child's process PID. */
    atomic_int_fast32_t pid;

    /* A varible to store the address of where to jump back to in the child
    process on signals. */
    jmp_buf return_jump;

    /* This is the number used to identify and choose the syscall that's going to be tested. */
    unsigned int syscall_number;

    /* This index is where we store the arguments we generate. */
    unsigned long arg_value_index[7];

    struct timeval time_of_syscall;

};

private extern int get_child_index_number(void);

private extern void create_syscall_children(void);

private extern void manage_syscall_children(void);

private extern void create_file_children(void);

private extern void manage_file_children(void);

#endif
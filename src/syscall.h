

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

#ifndef SYSCALL_H
#define SYSCALL_H

#include "private.h"
#include "stdatomic.h"
#include "entry.h"

#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <sys/time.h>

extern unsigned int number_of_children;

#define for_each_child(child, i) for(i = 0; i < number_of_children; i++) { child = get_child_from_index(i); }

private extern struct syscall_entry_shadow *get_entry(unsigned int syscall_number);

private extern struct child_ctx *get_child_from_index(unsigned int i);

private extern struct child_ctx *get_child_ctx(void);

private extern struct syscall_table_shadow *get_syscall_table(void);

private extern int cleanup_syscall_table(void);

private extern int pick_syscall(struct child_ctx *ctx);

private extern int generate_arguments(struct child_ctx *ctx);

private extern int test_syscall(struct child_ctx *ctx);

private extern void create_syscall_children(void);

private extern void kill_all_children(void);

private extern int setup_syscall_module(void);

private extern void start_main_syscall_loop(void);

#endif

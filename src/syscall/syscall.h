/*
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

/**
*     @file syscall.h
*     @brief This file is the interface to the syscall module.
*
*     @author Harrison Bowden
*/

#ifndef SYSCALL_H
#define SYSCALL_H

#include "concurrent/concurrent.h"
#include "concurrent/epoch.h"

#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <setjmp.h>

struct child_ctx;

enum child_state {EMPTY};

/**
*    This function places the total number of syscalls available
*    on the current platform in the parameter total. This includes 
*    syscalls that have been turned off.
*    @param total The variable to store the total number of syscalls.
*/
extern void get_total_syscalls(uint32_t *total);

extern struct child_ctx *get_child_from_index(uint32_t i);

extern struct child_ctx *get_child(void);

extern struct child_ctx *get_child_ctx_from_pid(pid_t pid);

extern struct syscall_table *get_syscall_table(void);

extern void cleanup_syscall_table(struct syscall_table **table);

extern int32_t pick_syscall(struct child_ctx *ctx);

extern int32_t generate_arguments(struct child_ctx *ctx);

extern int32_t test_syscall(struct child_ctx *ctx);

extern void jump(struct child_ctx *child);

extern void set_child_pid(struct child_ctx *child, int32_t pid);

extern void set_had_error(struct child_ctx *child, int32_t val);

extern void set_ret_value(struct child_ctx *child, int32_t val);

extern void set_sig_num(struct child_ctx *child, int32_t num);

extern void set_did_jump(struct child_ctx *child, int32_t val);

extern void set_arg_size(struct child_ctx *child, uint64_t size);

extern int32_t get_arg_size(struct child_ctx *child, uint32_t arg_num, uint64_t *size);

extern uint32_t get_current_arg(struct child_ctx *child);

extern void kill_all_children(void);

extern struct syscall_entry *get_entry(uint32_t syscall_number);

extern int32_t setup_syscall_module(int32_t *stop_ptr, 
	                                uint32_t *counter,
	                                int32_t run_mode,
	                                epoch_ctx *e);

extern void start_main_syscall_loop(struct thread_ctx *thread);

#endif

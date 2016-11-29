/**
 * Copyright (c) 2016, Harrison Bowden, Secure Labs, Minneapolis, MN
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

#ifndef SYSCALL_LIST_H
#define SYSCALL_LIST_H

#define SYS_creat 0

#include "../arg_types.h"
#include "../entry.h"
#include "../generate.h"

/* Window's system call list. This file declares all the syscalls entries
on the Window's platform. These syscall entries tell us how to fuzz each syscall on the system.  */

/* Skip the fork syscall because it can't be called from syscall().  */

extern struct syscall_entry entry_read;

#endif

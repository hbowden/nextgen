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

#ifndef SYSCALL_TABLE_H
#define SYSCALL_TABLE_H

#include "entry.h"

#include <stdint.h>

struct syscall_table
{
    uint32_t number_of_syscalls;
    const char padding[4];
    struct syscall_entry *sys_entry;
};

struct syscall_table_shadow
{
	uint32_t number_of_syscalls;
    const char padding[4];
    struct syscall_entry_shadow **sys_entry;

};

extern struct syscall_table *get_table(void);

extern struct syscall_table freebsd_syscall_table[];

extern struct syscall_table mac_osx_syscall_table[];

#endif

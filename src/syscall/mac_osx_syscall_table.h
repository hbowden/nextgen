

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

#ifndef MAC_OSX_SYSCALL_TABLE_H
#define MAC_OSX_SYSCALL_TABLE_H

#include "syscall_list.h"
#include "syscall_table.h"

struct syscall_table mac_osx_syscall_table[] = {
    
    { .number_of_syscalls = 11 },
    { .sys_entry = &entry_read },
    { .sys_entry = &entry_write },
    { .sys_entry = &entry_open },
    { .sys_entry = &entry_close },
    { .sys_entry = &entry_wait4 },
    { .sys_entry = &entry_link },
    { .sys_entry = &entry_unlink },
    { .sys_entry = &entry_chdir },
    { .sys_entry = &entry_fchdir },
    { .sys_entry = &entry_mknod },
    { .sys_entry = &entry_getfsstat },
    
};

#endif

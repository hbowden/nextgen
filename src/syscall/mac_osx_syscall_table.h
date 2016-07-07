

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

struct syscall_table mac_osx_syscall_table = {
    
    .total_syscalls = 21,
    .sys_entry[0] = &entry_read,
    .sys_entry[1] = &entry_write,
    .sys_entry[2] = &entry_open,
    .sys_entry[3] = &entry_close,
    .sys_entry[4] = &entry_wait4,
    .sys_entry[5] = &entry_link,
    .sys_entry[6] = &entry_unlink,
    .sys_entry[7] = &entry_chdir,
    .sys_entry[8] = &entry_fchdir,
    .sys_entry[9] = &entry_mknod,
    .sys_entry[10] = &entry_getfsstat,
    .sys_entry[11] = &entry_setuid,
    .sys_entry[12] = &entry_ptrace,
    .sys_entry[13] = &entry_recvmsg,
    .sys_entry[14] = &entry_sendmsg,
    .sys_entry[15] = &entry_recvfrom,
    .sys_entry[16] = &entry_accept,
    .sys_entry[17] = &entry_getpeername,
    .sys_entry[18] = &entry_getsockname,
    .sys_entry[19] = &entry_access,
    .sys_entry[20] = &entry_chflags,
};

#endif

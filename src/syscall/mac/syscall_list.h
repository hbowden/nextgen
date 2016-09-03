

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

#ifndef SYSCALL_LIST_H
#define SYSCALL_LIST_H

#include "../arg_types.h"
#include "../generate.h"
#include "../entry.h"

#include <sys/syscall.h>
#include <unistd.h>

/* Mac OSX's system call list. This file declares all the syscalls entries
on the Mac OSX platform. These syscall entries tell us how to fuzz each syscall on the system.  */

extern struct syscall_entry entry_read;

extern struct syscall_entry entry_write;

extern struct syscall_entry entry_open;

extern struct syscall_entry entry_close;

extern struct syscall_entry entry_wait4;

extern struct syscall_entry entry_link;

extern struct syscall_entry entry_unlink;

extern struct syscall_entry entry_chdir;

extern struct syscall_entry entry_fchdir;

extern struct syscall_entry entry_mknod;

extern struct syscall_entry entry_chmod;

extern struct syscall_entry entry_getfsstat;

extern struct syscall_entry entry_lseek;

extern struct syscall_entry entry_mount;

extern struct syscall_entry entry_unmount;

extern struct syscall_entry entry_setuid;

extern struct syscall_entry entry_ptrace;

extern struct syscall_entry entry_recvmsg;

extern struct syscall_entry entry_sendmsg;

extern struct syscall_entry entry_recvfrom;

extern struct syscall_entry entry_accept;

extern struct syscall_entry entry_getpeername;

extern struct syscall_entry entry_getsockname;

extern struct syscall_entry entry_access;

extern struct syscall_entry entry_chflags;

extern struct syscall_entry entry_fchflags;

extern struct syscall_entry entry_kill;

extern struct syscall_entry entry_stat;

extern struct syscall_entry entry_lstat;

extern struct syscall_entry entry_dup;

extern struct syscall_entry entry_profile;

extern struct syscall_entry entry_ktrace;

extern struct syscall_entry entry_sigaction;

extern struct syscall_entry entry_sigprocmask;

extern struct syscall_entry entry_getlogin;

extern struct syscall_entry entry_setlogin;

extern struct syscall_entry entry_acct;

extern struct syscall_entry entry_sigpending;

extern struct syscall_entry entry_sigaltstack;

extern struct syscall_entry entry_ioctl;

#endif

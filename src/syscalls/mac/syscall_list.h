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

#ifndef SYSCALL_LIST_H
#define SYSCALL_LIST_H

#include "../../private.h"
#include "../../arg_types.h"
#include "../../entry.h"

#include <sys/syscall.h>
#include <unistd.h>

/* Mac OSX's system call list. This file declares all the syscalls entries
on the Mac OSX platform. These syscall entries tell us how to fuzz each syscall on the system.  */

private extern struct syscall_entry entry_read;

private extern struct syscall_entry entry_write;

private extern struct syscall_entry entry_open;

private extern struct syscall_entry entry_close;

private extern struct syscall_entry entry_wait4;

private extern struct syscall_entry entry_link;

private extern struct syscall_entry entry_unlink;

private extern struct syscall_entry entry_chdir;

private extern struct syscall_entry entry_fchdir;

private extern struct syscall_entry entry_mknod;

private extern struct syscall_entry entry_chmod;

private extern struct syscall_entry entry_getfsstat;

private extern struct syscall_entry entry_lseek;

private extern struct syscall_entry entry_mount;

private extern struct syscall_entry entry_unmount;

private extern struct syscall_entry entry_setuid;

private extern struct syscall_entry entry_ptrace;

private extern struct syscall_entry entry_recvmsg;

private extern struct syscall_entry entry_sendmsg;

private extern struct syscall_entry entry_recvfrom;

private extern struct syscall_entry entry_accept;

private extern struct syscall_entry entry_getpeername;

private extern struct syscall_entry entry_getsockname;

private extern struct syscall_entry entry_access;

private extern struct syscall_entry entry_chflags;

private extern struct syscall_entry entry_fchflags;

private extern struct syscall_entry entry_kill;

private extern struct syscall_entry entry_stat;

private extern struct syscall_entry entry_lstat;

private extern struct syscall_entry entry_dup;

private extern struct syscall_entry entry_profile;

private extern struct syscall_entry entry_ktrace;

private extern struct syscall_entry entry_sigaction;

private extern struct syscall_entry entry_sigprocmask;

private extern struct syscall_entry entry_getlogin;

private extern struct syscall_entry entry_setlogin;

private extern struct syscall_entry entry_acct;

private extern struct syscall_entry entry_sigpending;

private extern struct syscall_entry entry_sigaltstack;

private extern struct syscall_entry entry_ioctl;

#endif

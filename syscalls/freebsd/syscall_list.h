

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

#include "private.h"

private extern struct syscall_entry entry_read;

private extern struct syscall_entry entry_write;

private extern struct syscall_entry entry_open;

private extern struct syscall_entry entry_close;

private extern struct syscall_entry entry_wait4;

private extern struct syscall_entry entry_creat;

private extern struct syscall_entry entry_link;

private extern struct syscall_entry entry_unlink;

private extern struct syscall_entry entry_chdir;

private extern struct syscall_entry entry_fchdir;

private extern struct syscall_entry entry_mknod;

private extern struct syscall_entry entry_getfsstat;

private extern struct syscall_entry entry_lseek;

private extern struct syscall_entry entry_mount;

private extern struct syscall_entry entryUnmount;

private extern struct syscall_entry entrySetuid;

private extern struct syscall_entry entryPtrace;

private extern struct syscall_entry entryRecvmsg;

private extern struct syscall_entry entrySendmsg;

private extern struct syscall_entry entryRecvfrom;

private extern struct syscall_entry entryAccept;

private extern struct syscall_entry entryGetPeername;

private extern struct syscall_entry entryGetSockname;

private extern struct syscall_entry entryAccess;

private extern struct syscall_entry entryChflags;

private extern struct syscall_entry entryFchflags;

private extern struct syscall_entry entryKill;

private extern struct syscall_entry entryStat;

private extern struct syscall_entry entryLstat;

private extern struct syscall_entry entryDup;

private extern struct syscall_entry entryProfil;

private extern struct syscall_entry entryKtrace;

private extern struct syscall_entry entrySigaction;

private extern struct syscall_entry entrySigProcMask;

private extern struct syscall_entry entryGetlogin;

private extern struct syscall_entry entrySetlogin;

private extern struct syscall_entry entryAcct;

private extern struct syscall_entry entrySigpending;

private extern struct syscall_entry entrySigaltstack;

private extern struct syscall_entry entryIoctl;

#endif
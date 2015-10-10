

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

#ifndef GENERATE_H
#define GENERATE_H

#include "private.h"
#include "child.h"

private extern int generate_fd(unsigned long **fd, struct child_ctx *ctx);

private extern int generate_socket(unsigned long **sock, struct child_ctx *ctx);

private extern int generate_buf(unsigned long **buf, struct child_ctx *ctx);

private extern int generate_length(unsigned long **length, struct child_ctx *ctx);

private extern int generate_path(unsigned long **path, struct child_ctx *ctx);

private extern int generate_mode(unsigned long **mode, struct child_ctx *ctx);

private extern int generate_open_flag(unsigned long **flag, struct child_ctx *ctx);

private extern int generate_fs_stat(unsigned long **stat, struct child_ctx *ctx);

private extern int generate_fs_stat_flag(unsigned long **flag, struct child_ctx *ctx);

private extern int generate_pid(unsigned long **pid, struct child_ctx *ctx);

private extern int generate_int(unsigned long **integer, struct child_ctx *ctx);

private extern int generate_rusage(unsigned long **usage, struct child_ctx *ctx);

private extern int generate_wait_option(unsigned long **option, struct child_ctx *ctx);

private extern int generate_whence(unsigned long **whence, struct child_ctx *ctx);

private extern int generate_offset(unsigned long **offset, struct child_ctx *ctx);

private extern int generate_mount_type(unsigned long **type, struct child_ctx *ctx);

private extern int generate_dirpath(unsigned long **dirpath, struct child_ctx *ctx);

private extern int generate_mount_flags(unsigned long **flag, struct child_ctx *ctx);

private extern int generate_unmount_flags(unsigned long **flag, struct child_ctx *ctx);

private extern int generate_request(unsigned long **flag, struct child_ctx *ctx);

private extern int generate_recv_flags(unsigned long **flag, struct child_ctx *ctx);

private extern int generate_mount_path(unsigned long **path, struct child_ctx *ctx);

private extern int generate_dev(unsigned long **dev, struct child_ctx *ctx);

#endif

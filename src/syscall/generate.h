

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

#ifndef GENERATE_H
#define GENERATE_H

#include "syscall.h"

#include <stdint.h>

extern int32_t generate_fd(uint64_t **fd, struct child_ctx *ctx);

extern int32_t generate_socket(uint64_t **sock, struct child_ctx *ctx);

extern int32_t generate_buf(uint64_t **buf, struct child_ctx *ctx);

extern int32_t generate_length(uint64_t **length, struct child_ctx *ctx);

extern int32_t generate_path(uint64_t **path, struct child_ctx *ctx);

extern int32_t generate_mode(uint64_t **mode, struct child_ctx *ctx);

extern int32_t generate_open_flag(uint64_t **flag, struct child_ctx *ctx);

/* Don't compile on Linux. */
#ifndef LINUX

extern int32_t generate_fs_stat(uint64_t **stat, struct child_ctx *ctx);

#endif

extern int32_t generate_fs_stat_flag(uint64_t **flag, struct child_ctx *ctx);

extern int32_t generate_pid(uint64_t **pid, struct child_ctx *ctx);

extern int32_t generate_int(uint64_t **integer, struct child_ctx *ctx);

extern int32_t generate_rusage(uint64_t **usage, struct child_ctx *ctx);

extern int32_t generate_wait_option(uint64_t **option, struct child_ctx *ctx);

extern int32_t generate_whence(uint64_t **whence, struct child_ctx *ctx);

extern int32_t generate_offset(uint64_t **offset, struct child_ctx *ctx);

extern int32_t generate_mount_type(uint64_t **type, struct child_ctx *ctx);

extern int32_t generate_dirpath(uint64_t **dirpath, struct child_ctx *ctx);

extern int32_t generate_mount_flags(uint64_t **flag, struct child_ctx *ctx);

extern int32_t generate_unmount_flags(uint64_t **flag, struct child_ctx *ctx);

extern int32_t generate_request(uint64_t **flag, struct child_ctx *ctx);

extern int32_t generate_recv_flags(uint64_t **flag, struct child_ctx *ctx);

extern int32_t generate_mountpath(uint64_t **path, struct child_ctx *ctx);

extern int32_t generate_dev(uint64_t **dev, struct child_ctx *ctx);

extern int32_t generate_message(uint64_t **msg, struct child_ctx *ctx);

extern int32_t generate_send_flags(uint64_t **flag, struct child_ctx *ctx);

extern int32_t generate_sockaddr(uint64_t **addr, struct child_ctx *ctx);

extern int32_t generate_socklen(uint64_t **len, struct child_ctx *ctx);

extern int32_t generate_amode(uint64_t **amode, struct child_ctx *ctx);

#ifndef LINUX

extern int32_t generate_chflags(uint64_t **flag, struct child_ctx *ctx);

#endif

#endif

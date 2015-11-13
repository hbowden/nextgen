

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

#include "syscall.h"

extern int32_t generate_fd(uint64_t **fd);

extern int32_t generate_socket(uint64_t **sock);

extern int32_t generate_buf(uint64_t **buf);

extern int32_t generate_length(uint64_t **length);

extern int32_t generate_path(uint64_t **path);

extern int32_t generate_mode(uint64_t **mode);

extern int32_t generate_open_flag(uint64_t **flag);

extern int32_t generate_fs_stat(uint64_t **stat);

extern int32_t generate_fs_stat_flag(uint64_t **flag);

extern int32_t generate_pid(uint64_t **pid);

extern int32_t generate_int(uint64_t **integer);

extern int32_t generate_rusage(uint64_t **usage);

extern int32_t generate_wait_option(uint64_t **option);

extern int32_t generate_whence(uint64_t **whence);

extern int32_t generate_offset(uint64_t **offset);

extern int32_t generate_mount_type(uint64_t **type);

extern int32_t generate_dirpath(uint64_t **dirpath);

extern int32_t generate_mount_flags(uint64_t **flag);

extern int32_t generate_unmount_flags(uint64_t **flag);

extern int32_t generate_request(uint64_t **flag);

extern int32_t generate_recv_flags(uint64_t **flag);

extern int32_t generate_mount_path(uint64_t**path);

extern int32_t generate_dev(uint64_t **dev);

#endif



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

#include "depend-inject/depend-inject.h"

#include <stdint.h>

extern void inject_generate_deps(struct dependency_context *ctx);

extern int32_t generate_int(uint64_t **num);

extern int32_t generate_mach_port(uint64_t **ptr);

extern int32_t generate_ptr(uint64_t **ptr);

extern int32_t generate_fd(uint64_t **fd);

extern int32_t generate_socket(uint64_t **sock);

extern int32_t generate_buf(uint64_t **buf);

extern int32_t generate_pid(uint64_t **pid);

extern int32_t generate_path(uint64_t **path);

#endif

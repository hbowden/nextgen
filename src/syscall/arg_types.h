

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

#ifndef ARG_TYPES_H
#define ARG_TYPES_H

#include <stdint.h>
#include "utils/utils.h"

enum arg_type { ADDRESS, INT, PID };

struct arg_context
{
	  const char *name;
	  enum arg_type type;
	  enum yes_no should_free;
	  int32_t log_type;
    const char padding[4];
};

extern void inject_arg_types_deps(struct dependency_context *ctx);

extern struct arg_context *get_arg_context(enum arg_type type);

extern struct arg_context address_ctx;

extern struct arg_context int_ctx;

extern struct arg_context pid_ctx;

#endif

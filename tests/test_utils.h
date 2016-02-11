

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

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "../src/io.h"

#include <stdint.h>
#include <assert.h>
#include <setjmp.h>

enum log_type { DECLARE, SUCCESS, FAIL };

struct stats
{
    uint32_t fails;

    uint32_t successes;

    uint32_t test_ran;

    uint32_t asserts_ran;
};

extern struct stats *test_stat;

extern jmp_buf return_jump;

extern int32_t skip_test;

/* Assert macro but with a counter. */
#define assert_stat(arg) test_stat->asserts_ran++; assert(arg)

extern int32_t log_test(enum log_type type, const char *input);

extern struct stats *init_stats_obj(void);

extern struct stats *create_stats_obj(void);

/* Call once at the begining to setup the framework. */
extern int32_t init_test_framework(void);

#endif

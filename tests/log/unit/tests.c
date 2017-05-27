/*
 * Copyright (c) 2017, Harrison Bowden, Minneapolis, MN
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
 */

#include "unity.h"
#include "io/io.h"
#include "log/log.c"
#include "memory/memory.h"
#include "depend-inject/depend-inject.h"
#include <fcntl.h>

static void setup_tests(void)
{
    struct dependency_context *ctx = NULL;
    struct output_writter *output = NULL;

    output = get_console_writter();
    TEST_ASSERT_NOT_NULL(output);

    struct memory_allocator *allocator = NULL;

    allocator = get_default_allocator();
    TEST_ASSERT_NOT_NULL(allocator);

    ctx = create_dependency_ctx(create_dependency(output, OUTPUT),
                                create_dependency(allocator, ALLOCATOR),
                                NULL);
    inject_log_deps(ctx);
}

static void test_create_db(void)
{
	int32_t rtrn = 0;
	char *path = "/tmp/nextgen.nx";

	rtrn = create_db(path);
	TEST_ASSERT(rtrn == 0);
	TEST_ASSERT_NOT_NULL(db);

	int32_t fd = 0;
	fd = open(path, O_RDONLY);
	TEST_ASSERT(fd > 0);
}

static void test_log_ptr(void)
{
	// char *msg = "message";

	// log_ptr();
}

int main(void)
{
    setup_tests();
    test_create_db();
    test_log_ptr();

    return (0);
}
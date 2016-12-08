/*
 * Copyright (c) 2016, Harrison Bowden, Minneapolis, MN
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
#include "memory/memory.h"
#include "crypto/random.h"
#include "resource/resource.c"
#include "io/io.h"

static void test_resource_generator(void)
{
	  int32_t fd = 0;
		struct resource_generator *rsrc_gen = NULL;

		rsrc_gen = get_resource_generator();
		TEST_ASSERT_NOT_NULL(rsrc_gen);

		fd = rsrc_gen->get_desc();
    TEST_ASSERT(fd > 0);

	  return;
}

static void test_inject_resource_deps(void)
{
	  struct dependency_context *ctx = NULL;
	  struct output_writter *output_writter = get_console_writter();
	  TEST_ASSERT_NOT_NULL(output_writter);

	  struct memory_allocator *mem_allocator = get_default_allocator();
	  TEST_ASSERT_NOT_NULL(mem_allocator);

		ctx = create_dependency_ctx(create_dependency(output_writter, OUTPUT),
																create_dependency(mem_allocator, ALLOCATOR),
															  NULL);
		TEST_ASSERT_NOT_NULL(ctx->array);

    /* We need to inject the utils and crypto modules deps before using the
		  resource module. */
    inject_utils_deps(ctx);
		inject_crypto_deps(ctx);

		struct random_generator *random = get_default_random_generator();
		TEST_ASSERT_NOT_NULL(random);

		ctx = create_dependency_ctx(create_dependency(output_writter, OUTPUT),
																create_dependency(mem_allocator, ALLOCATOR),
																create_dependency(random, RANDOM_GEN),
															  NULL);
		TEST_ASSERT_NOT_NULL(ctx->array);

    inject_utils_deps(ctx);
		inject_resource_deps(ctx);
}

int main(void)
{
	  test_inject_resource_deps();
    test_resource_generator();

	  _exit(0);
}

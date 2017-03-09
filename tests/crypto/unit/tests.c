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
#include "io/io.h"
#include "utils/autofree.h"
#include "crypto/crypto.h"
#include "crypto/random.h"
#include "crypto/hash.h"
#include "memory/memory.h"

#include <string.h>

static uint32_t iterations = 1000;

static void test_get_random_generator(void)
{
    struct random_generator *random = NULL;
	struct memory_allocator *allocator = NULL;
	struct output_writter *output = NULL;

	output = get_console_writter();
	TEST_ASSERT_NOT_NULL(output);

	allocator = get_default_allocator();
	TEST_ASSERT_NOT_NULL(allocator);

	random = get_default_random_generator();
	TEST_ASSERT_NOT_NULL(random);
	TEST_ASSERT_NOT_NULL(random->range);

	int32_t rtrn = 0;
	uint32_t number = 0;
	uint32_t i = 0;

	for(i = 0; i < iterations; i++)
	{
		rtrn = random->range(1000, &number);
		TEST_ASSERT(rtrn == 0);
		TEST_ASSERT(number <= 1000);

        char *buf auto_free = NULL;

        rtrn = random->bytes(&buf, number);
        TEST_ASSERT(rtrn == 0);
        TEST_ASSERT_NOT_NULL(buf);
        TEST_ASSERT(strlen(buf) <= number);
	}
}

static void test_get_hasher(void)
{
    struct hasher *hasher = NULL;
    struct memory_allocator *allocator = NULL;
    struct random_generator *random_gen = NULL;

    hasher = get_hasher();
    TEST_ASSERT_NOT_NULL(hasher);

    random_gen = get_default_random_generator();
    TEST_ASSERT_NOT_NULL(random_gen);

    allocator = get_default_allocator();
    TEST_ASSERT_NOT_NULL(allocator);

    uint32_t i;
    int32_t rtrn = 0;
    char *buf = NULL;
    char *hash = NULL;

    for(i = 0; i < iterations; i++)
    {
        rtrn = random_gen->bytes(&buf, 1024);
        TEST_ASSERT(rtrn == 0);
        TEST_ASSERT_NOT_NULL(buf);

        rtrn = hasher->sha256(buf, &hash);
        TEST_ASSERT(rtrn == 0);
        TEST_ASSERT_NOT_NULL(hash);

        TEST_ASSERT(strlen(hash) == 64);

        allocator->free((void **)&hash);
        allocator->free((void **)&buf);
    }
}

static void setup_test(void)
{
    struct memory_allocator *allocator = NULL;
    struct dependency_context *ctx = NULL;
    struct output_writter *output = NULL;

    output = get_console_writter();
    TEST_ASSERT_NOT_NULL(output);

    allocator = get_default_allocator();
    TEST_ASSERT_NOT_NULL(allocator);

    ctx = create_dependency_ctx(create_dependency(output, OUTPUT),
                                create_dependency(allocator, ALLOCATOR),
                                NULL);
    TEST_ASSERT_NOT_NULL(ctx->array);
    TEST_ASSERT(ctx->count == 2);
    inject_crypto_deps(ctx);
}

int main(void)
{
    setup_test();

    test_get_random_generator();
    test_get_hasher();

	return (0);
}

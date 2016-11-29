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
#include "utils/autofree.h"
#include "crypto/crypto.h"
#include "memory/memory.h"

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

		random = get_default_random_generator(allocator, output);
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

        rtrn = random->bytes(allocator, output, &buf, number);
        TEST_ASSERT(rtrn == 0);
        TEST_ASSERT_NOT_NULL(buf);
        TEST_ASSERT(strlen(buf) <= number);
		}
}

int main(void)
{
    test_get_random_generator();

	  return (0);
}

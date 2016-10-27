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

static void test_default_memory_allocator(void)
{
    struct memory_allocator *allocator = NULL;
    allocator = get_default_allocator();
    TEST_ASSERT_NOT_NULL(allocator);
    TEST_ASSERT_NOT_NULL(&allocator->alloc);
    TEST_ASSERT_NOT_NULL(&allocator->shared);

    void *buf = allocator->alloc(100);
    TEST_ASSERT_NOT_NULL(buf);

    void *buf2 = allocator->alloc(0);
    TEST_ASSERT_NULL(buf2);

    void *buf3 = allocator->shared(0);
    TEST_ASSERT_NULL(buf3);

    void *buf4 = allocator->shared(100);
    TEST_ASSERT_NOT_NULL(buf4);
}

int main(void)
{
    test_default_memory_allocator();

	  return (0);
}

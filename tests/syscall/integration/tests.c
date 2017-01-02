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
#include "memory/memory.h"
#include "syscall/syscall.h"
#include "syscall/child.c"

#include <signal.h>

static void setup_tests(void)
{
  struct dependency_context *ctx = NULL;
  struct output_writter *output1 = NULL;

  output1 = get_console_writter();
  TEST_ASSERT_NOT_NULL(output1);

  struct memory_allocator *allocator1 = NULL;

  allocator1 = get_default_allocator();
  TEST_ASSERT_NOT_NULL(allocator1);

  ctx = create_dependency_ctx(create_dependency(output1, OUTPUT),
                              create_dependency(allocator1, ALLOCATOR),
                              NULL);
  TEST_ASSERT_NOT_NULL(ctx->array);
  TEST_ASSERT(ctx->count == 2);
  uint32_t i;
  for(i = 0; i < ctx->count; i++)
  {
      TEST_ASSERT_NOT_NULL(ctx->array[i]);
  }

  TEST_ASSERT(ctx->array[0]->name == OUTPUT);
  TEST_ASSERT(ctx->array[1]->name == ALLOCATOR);
  void *buf = NULL;

  struct memory_allocator *alloc = NULL;
  alloc = (struct memory_allocator *)ctx->array[1]->interface;
  TEST_ASSERT_NOT_NULL(alloc);
  buf = alloc->shared(10);
  TEST_ASSERT_NOT_NULL(buf);

  inject_syscall_deps(ctx);
}

static void test_create_test_case(void)
{
    struct test_case *test = NULL;

    test = create_test_case();
    TEST_ASSERT_NOT_NULL(test);

    return;
}

static void test_get_table(void)
{
    struct syscall_table *table = NULL;

    table = get_table();
    TEST_ASSERT_NOT_NULL(table);
}

int main(void)
{
    setup_tests();

    test_create_test_case();
    test_get_table();

    return (0);
}

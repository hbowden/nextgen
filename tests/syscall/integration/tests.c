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
#include "crypto/crypto.h"
#include "crypto/random.h"
#include "syscall/syscall.h"
#include "syscall/generate.h"
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

  inject_crypto_deps(ctx);

  struct random_generator *random_gen = NULL;

  random_gen = get_default_random_generator();
  TEST_ASSERT_NOT_NULL(random_gen);

  add_dep(ctx, create_dependency(random_gen, RANDOM_GEN));

  inject_syscall_deps(ctx);
}

static void test_create_test_case(void)
{
    struct test_case *test = NULL;

    test = create_test_case();
    TEST_ASSERT_NOT_NULL(test);
    // TEST_ASSERT_NOT_NULL(test->arg_value_array);

    return;
}

static void check_entry(struct syscall_entry *entry)
{
    TEST_ASSERT_NOT_NULL(entry);
    TEST_ASSERT(entry->total_args >= 0);
    TEST_ASSERT_NOT_NULL(entry->syscall_name);
    TEST_ASSERT_NOT_NULL(entry->return_type);

    uint32_t i;
    int32_t rtrn = 0;
    uint64_t *arg = NULL;

    for(i = 0; i < entry->total_args; i++)
    {
        if(entry->total_args != 0)
        {
            rtrn = entry->get_arg_array[i](&arg);
            TEST_ASSERT(rtrn == 0);
            TEST_ASSERT_NOT_NULL(arg);
            if(entry->arg_type_array[i] == PID)
            {
                TEST_ASSERT(kill((pid_t)(*arg), SIGKILL) == 0);
            }
        }
    }
}

static void test_get_table(void)
{
    struct syscall_table *table = NULL;

    table = get_table();
    TEST_ASSERT_NOT_NULL(table);
    TEST_ASSERT(table->total_syscalls > 0);

    uint32_t i;

    for(i = 0; i < table->total_syscalls; i++)
    {
        check_entry(table->sys_entry[i]);
    }
}

static void test_generate_ptr(void)
{
    void *ptr = NULL;

    int32_t rtrn = generate_ptr((uint64_t **)&ptr);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT_NOT_NULL(ptr);
}

static void test_generate_int(void)
{
    int64_t *num = NULL;
    int32_t rtrn = generate_int((uint64_t **)&num);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT_NOT_NULL(num);
}

static void test_generate_pid(void)
{
    pid_t *pid = NULL;
    int32_t rtrn = generate_pid((uint64_t **)&pid);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT_NOT_NULL(pid);
    TEST_ASSERT(kill((*pid), SIGKILL) == 0);
}

int main(void)
{
    setup_tests();

    test_create_test_case();
    test_generate_ptr();
    test_generate_int();
    test_generate_pid();
    test_get_table();

    return (0);
}

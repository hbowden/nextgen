/*
 * Copyright (c) 2016, Harrison Bowden, Minneapolis, MN
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice
 * and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY å
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "unity.h"
#include "io/io.h"
#include "utils/utils.h"
#include "utils/autofree.h"
#include "crypto/crypto.h"
#include "crypto/random.h"
#include "memory/memory.h"
#include "depend-inject/depend-inject.h"

static void test_create_dependency(void)
{
    struct dependency *dep = NULL;
    struct output_writter *output = NULL;

    output = get_console_writter();
    TEST_ASSERT_NOT_NULL(output);

    dep = create_dependency(output, OUTPUT);
    TEST_ASSERT_NOT_NULL(dep);
    TEST_ASSERT_NOT_NULL(dep->interface);
    TEST_ASSERT((int)dep->name > -1);
}

static void test_create_dependency_array(void)
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
    TEST_ASSERT_NOT_NULL(ctx->array);
    TEST_ASSERT(ctx->count == 2);
    uint32_t i;
    for(i = 0; i < ctx->count; i++)
    {
        TEST_ASSERT_NOT_NULL(ctx->array[i]);
    }

    TEST_ASSERT(ctx->array[0]->name == OUTPUT);
    TEST_ASSERT(ctx->array[1]->name == ALLOCATOR);

    void (*write)(enum out_type type, const char *format, ...);
    write = (void (*)(enum out_type type, const char *format, ...)) ctx->array[0]->interface;

    return;
}

static void test_add_dep(void)
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
    TEST_ASSERT_NOT_NULL(ctx->array);
    TEST_ASSERT(ctx->count == 2);
    uint32_t i;
    for(i = 0; i < ctx->count; i++)
    {
        TEST_ASSERT_NOT_NULL(ctx->array[i]);
    }

    TEST_ASSERT(ctx->array[0]->name == OUTPUT);
    TEST_ASSERT(ctx->array[1]->name == ALLOCATOR);

    inject_crypto_deps(ctx);

    struct random_generator *random_gen = NULL;
    random_gen = get_default_random_generator();
    TEST_ASSERT_NOT_NULL(random_gen);

    int32_t rtrn = 0;

    rtrn = add_dep(ctx, create_dependency(random_gen, RANDOM_GEN));
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT(ctx->array[2]->name == RANDOM_GEN);
}


int main(void)
{
    test_create_dependency();
    test_create_dependency_array();
    test_add_dep();

    _exit(0);
}

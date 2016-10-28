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

#include "runtime/runtime.h"
#include "runtime/fuzzer.h"
#include "memory/memory.h"
#include "crypto/crypto.h"
#include "io/io.h"
#include "unity.h"

static void test_get_syscall_fuzzer(void)
{
    int32_t rtrn = 0;
    struct fuzzer_instance *fuzzer = NULL;
    struct output_writter *output = get_console_writter();
    struct memory_allocator *allocator = get_default_allocator();

    fuzzer = get_syscall_fuzzer(allocator, output);
    TEST_ASSERT_NOT_NULL(fuzzer);
    TEST_ASSERT_NOT_NULL(fuzzer->setup);
    TEST_ASSERT_NOT_NULL(fuzzer->start);
    TEST_ASSERT_NOT_NULL(fuzzer->stop);

    struct random_generator *random = NULL;
    random = get_default_random_generator(allocator, output);

    rtrn = fuzzer->setup(random, output);
    TEST_ASSERT(rtrn == 0);

    return;
}

static void test_get_fuzzer(void)
{
    struct fuzzer_config *config = NULL;
    struct fuzzer_instance *fuzzer = NULL;

    fuzzer = get_fuzzer(config);
    TEST_ASSERT_NOT_NULL(fuzzer);

    return;
}

static void test_get_os(void)
{
    const char *os = get_os();
    TEST_ASSERT_NOT_NULL(os);

    return;
}

int main(void)
{
    test_get_os();
    test_get_syscall_fuzzer();

    return (0);
}

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

#include "runtime/nextgen.c" // Include the .c file so we can test static functions.
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

    fuzzer = get_syscall_fuzzer("/tmp", "/tmp/results", allocator, output);
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
    //int32_t rtrn = 0;
    struct fuzzer_config *config = NULL;
    struct fuzzer_instance *fuzzer = NULL;
    struct output_writter *output = NULL;
    struct memory_allocator *allocator = NULL;

    output = get_console_writter();
    TEST_ASSERT_NOT_NULL(output);

    allocator = get_default_allocator();
    TEST_ASSERT_NOT_NULL(allocator);

    /* Return NULL when config is NULL. */
    fuzzer = get_fuzzer(config, allocator, output);
    TEST_ASSERT_NULL(fuzzer);

    config = init_fuzzer_config(output, allocator);
    TEST_ASSERT_NOT_NULL(config);

    /* Should return NULL because no options were set on
    the fuzzer config. */
    fuzzer = get_fuzzer(config, allocator, output);
    TEST_ASSERT_NULL(fuzzer);

    set_fuzz_mode(config, MODE_SYSCALL, output);

    /* Should return NULL because no output path are selected. */
    fuzzer = get_fuzzer(config, allocator, output);
    TEST_ASSERT_NULL(fuzzer);

    // rtrn = fuzzer->start();
    // TEST_ASSERT(rtrn > -1);

    return;
}

static void test_init_fuzzer_config(void)
{
    struct fuzzer_config *config = NULL;
    struct output_writter *output = NULL;
    struct memory_allocator *allocator = NULL;

    output = get_console_writter();
    TEST_ASSERT_NOT_NULL(output);

    allocator = get_default_allocator();
    TEST_ASSERT_NOT_NULL(allocator);

    config = init_fuzzer_config(output, allocator);
    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_NULL(config->exec_path);
    TEST_ASSERT_NULL(config->input_path);
    TEST_ASSERT_NULL(config->output_path);
    TEST_ASSERT_NULL(config->args);
}

static void test_set_fuzz_mode(void)
{
    int32_t rtrn = 0;
    struct fuzzer_config *config = NULL;
    struct output_writter *output = NULL;
    struct memory_allocator *allocator = NULL;

    output = get_console_writter();
    TEST_ASSERT_NOT_NULL(output);

    allocator = get_default_allocator();
    TEST_ASSERT_NOT_NULL(allocator);

    config = init_fuzzer_config(output, allocator);
    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_NULL(config->exec_path);
    TEST_ASSERT_NULL(config->input_path);
    TEST_ASSERT_NULL(config->output_path);
    TEST_ASSERT_NULL(config->args);

    /* Should fail when passed unknown fuzz mode. */
    rtrn = set_fuzz_mode(config, (enum fuzz_mode)99, output);
    TEST_ASSERT(rtrn == -1);

    rtrn = set_fuzz_mode(config, MODE_SYSCALL, output);
    TEST_ASSERT(rtrn == 0);

    return;
}

int main(void)
{
    test_set_fuzz_mode();
    test_init_fuzzer_config();
    test_get_syscall_fuzzer();
    test_get_fuzzer();

    return (0);
}

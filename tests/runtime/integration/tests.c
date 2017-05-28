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
#include "runtime/runtime.h"
#include "memory/memory.h"
#include "depend-inject/depend-inject.h"
#include "io/io.h"
#include "unity.h"

static void test_get_syscall_fuzzer(void)
{
    int32_t rtrn = 0;
    struct fuzzer_instance *fuzzer = NULL;

    fuzzer = get_syscall_fuzzer("/tmp/results");
    TEST_ASSERT_NOT_NULL(fuzzer);
    TEST_ASSERT_NOT_NULL(fuzzer->setup);
    TEST_ASSERT_NOT_NULL(fuzzer->start);
    TEST_ASSERT_NOT_NULL(fuzzer->stop);

    rtrn = fuzzer->setup();
    TEST_ASSERT(rtrn == 0);

    return;
}

static void test_get_fuzzer(void)
{
    int32_t rtrn = 0;
    struct fuzzer_config *config = NULL;
    struct fuzzer_instance *fuzzer = NULL;

    /* Return NULL when config is NULL. */
    fuzzer = get_fuzzer(config);
    TEST_ASSERT_NULL(fuzzer);

    config = init_fuzzer_config();
    TEST_ASSERT_NOT_NULL(config);

    /* Should return NULL because no options were set on the fuzzer config. */
    fuzzer = get_fuzzer(config);
    TEST_ASSERT_NULL(fuzzer);

    set_fuzz_mode(config, MODE_SYSCALL);

    /* Should return NULL because no output path are selected. */
    fuzzer = get_fuzzer(config);
    TEST_ASSERT_NULL(fuzzer);

    set_output_path(config, "/tmp/results/");

    fuzzer = get_fuzzer(config);
    TEST_ASSERT_NOT_NULL(fuzzer);

    rtrn = fuzzer->setup();
    TEST_ASSERT(rtrn == 0);

    // rtrn = fuzzer->start(output, allocator);
    // TEST_ASSERT(rtrn == 0);
    //
    // rtrn = fuzzer->stop();
    // TEST_ASSERT(rtrn == 0);

    return;
}

static void test_init_fuzzer_config(void)
{
    struct fuzzer_config *config = NULL;

    config = init_fuzzer_config();
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

    config = init_fuzzer_config();
    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_NULL(config->exec_path);
    TEST_ASSERT_NULL(config->input_path);
    TEST_ASSERT_NULL(config->output_path);
    TEST_ASSERT_NULL(config->args);

    /* Should fail when passed unknown fuzz mode. */
    rtrn = set_fuzz_mode(config, (enum fuzz_mode)99);
    TEST_ASSERT(rtrn == -1);

    rtrn = set_fuzz_mode(config, MODE_SYSCALL);
    TEST_ASSERT(rtrn == 0);

    return;
}

static void test_init_fuzzer_control(void)
{
    struct fuzzer_control *control = NULL;

    control = init_fuzzer_control();
    TEST_ASSERT_NOT_NULL(control);
    TEST_ASSERT(control->stop == FALSE);

    return;
}

static int32_t inject_runtime_deps(struct dependency_context *ctx)
{
    inject_fuzzer_deps(ctx);
    inject_syscall_fuzzer_deps(ctx);
    inject_nextgen_deps(ctx);

    return (0);
}

static void setup_tests(void)
{
    struct output_writter *output_writter = NULL;
    struct memory_allocator *mem_allocator = NULL;

    output_writter = get_console_writter();
    TEST_ASSERT_NOT_NULL(output_writter);

    mem_allocator = get_default_allocator();
    TEST_ASSERT_NOT_NULL(mem_allocator);

    struct dependency_context *ctx = NULL;

    ctx = create_dependency_ctx(create_dependency(output_writter, OUTPUT),
                                create_dependency(mem_allocator, ALLOCATOR),
                                NULL);
    TEST_ASSERT_NOT_NULL(ctx);

    inject_runtime_deps(ctx);
}

static void test_get_verbosity(void)
{
    int32_t verbose = 9;
    verbose = get_verbosity();

    TEST_ASSERT(verbose != 9);
}

static void test_set_verbosity(void)
{
    int32_t verbose = 9;
    set_verbosity(verbose);

    TEST_ASSERT(get_verbosity() == 9);
}

int main(void)
{
    setup_tests();
    test_get_verbosity();
    test_set_verbosity();
    test_set_fuzz_mode();
    test_init_fuzzer_config();
    test_get_syscall_fuzzer();
    test_get_fuzzer();
    test_init_fuzzer_control();

    return (0);
}

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
#include "syscall/child.c"

static void test_create_children_state(void)
{
    uint32_t total_children = 8;
    struct children_state *child_state = NULL;

    struct output_writter *output = get_console_writter();
    struct memory_allocator *allocator = get_default_allocator();

    child_state = create_children_state(allocator, output, total_children);
    TEST_ASSERT_NOT_NULL(child_state);
    TEST_ASSERT_NOT_NULL(child_state->children);

    uint32_t i;

    for(i = 0; i < total_children; i++)
    {
        TEST_ASSERT_NOT_NULL(child_state->children[i]);
    }

    TEST_ASSERT(child_state->total_children == total_children);
    TEST_ASSERT(child_state->test_counter == 0);
    TEST_ASSERT(child_state->running_children == 0);
}

static void test_create_syscall_child(void)
{
    uint32_t total_children = 8;
    struct syscall_child *child = NULL;
    struct children_state *child_state = NULL;
    struct output_writter *output = get_console_writter();
    struct memory_allocator *allocator = get_default_allocator();

    child_state = create_children_state(allocator, output, total_children);
    TEST_ASSERT_NOT_NULL(child_state);

    child = create_syscall_child(child_state);
    TEST_ASSERT_NOT_NULL(child);

    uint32_t i;
    uint32_t counter = 0;

    for(i = 0; i < total_children; i++)
    {
        if(atomic_load_int32(&child_state->children[i]->pid) == INITIALIZING)
            counter++;
    }

    TEST_ASSERT(counter == 1);

    return;
}

int main(void)
{
    test_create_children_state();
    test_create_syscall_child();
    return (0);
}

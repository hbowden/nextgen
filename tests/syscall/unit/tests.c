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

static void test_create_children_state(void)
{
    uint32_t total_children = 8;
    struct children_state *children_state = NULL;

    children_state = create_children_state(total_children);
    TEST_ASSERT_NOT_NULL(children_state);

    uint32_t i;

    for(i = 0; i < total_children; i++)
    {
        TEST_ASSERT_NOT_NULL(children_state->children[i]);
    }

    TEST_ASSERT(children_state->total_children == total_children);
    TEST_ASSERT(children_state->test_counter == 0);
    TEST_ASSERT(children_state->running_children == 0);
}

static void test_create_syscall_child(void)
{
    int32_t rtrn = 0;
    uint32_t total_children = 8;
    struct syscall_child *child = NULL;
    struct children_state *children_state = NULL;

    children_state = create_children_state(total_children);
    TEST_ASSERT_NOT_NULL(children_state);

    set_children_state(children_state);

    child = create_syscall_child();
    TEST_ASSERT_NOT_NULL(child);

    uint32_t i;
    uint32_t counter = 0;

    for(i = 0; i < total_children; i++)
    {
        if(atomic_load_int32(&children_state->children[i]->pid) == INITIALIZING)
            counter++;
    }

    TEST_ASSERT(counter == 1);

    rtrn = child->start(child);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT(atomic_load_int32(&child->pid) > 0);
    TEST_ASSERT(children_state->running_children == 1);

    rtrn = kill(child->pid, SIGKILL);
    TEST_ASSERT(rtrn == 0);

    return;
}

static void test_set_children_state(void)
{
    uint32_t total_children = 8;
    struct children_state *children_state = NULL;

    children_state = create_children_state(total_children);
    TEST_ASSERT_NOT_NULL(children_state);

    TEST_ASSERT_NULL(state);
    set_children_state(children_state);
    TEST_ASSERT_NOT_NULL(state);

    return;
}

static void test_get_child_with_pid(void)
{
    int32_t rtrn = 0;
    uint32_t total_children = 8;
    struct children_state *children_state = NULL;

    children_state = create_children_state(total_children);
    TEST_ASSERT_NOT_NULL(children_state);

    set_children_state(children_state);
    TEST_ASSERT_NOT_NULL(state);

    struct syscall_child *child = NULL;
    struct syscall_child *test_child = NULL;

    child = create_syscall_child();
    TEST_ASSERT_NOT_NULL(child);

    rtrn = child->start(child);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT(atomic_load_int32(&child->pid) > 0);
    TEST_ASSERT(children_state->running_children == 1);

    test_child = get_child_with_pid(child->pid);
    TEST_ASSERT_NOT_NULL(test_child);
    TEST_ASSERT(child->pid == test_child->pid);

    rtrn = kill(child->pid, SIGKILL);
    TEST_ASSERT(rtrn == 0);

    return;
}

static void test_setup_ctrlc_handler(void)
{
    pid_t pid = 0;

    int32_t fd[2];

    int32_t rtrn = pipe(fd);
    TEST_ASSERT(rtrn == 0);

    pid = fork();
    if(pid == 0)
    {
        uint32_t total_children = 1;
        struct children_state *children_state = NULL;

        children_state = create_children_state(total_children);
        TEST_ASSERT_NOT_NULL(children_state);

        set_children_state(children_state);
        TEST_ASSERT_NOT_NULL(state);

        rtrn = setup_ctrlc_handler();
        TEST_ASSERT(rtrn == 0);

        struct syscall_child *child = NULL;

        child = create_syscall_child();
        TEST_ASSERT_NOT_NULL(child);

        rtrn = child->start(child);
        TEST_ASSERT(rtrn == 0);
        TEST_ASSERT(atomic_load_int32(&child->pid) > 0);
        TEST_ASSERT(children_state->running_children == 1);

        /* Let the parent process know it's safe to continue. */
        ssize_t ret = write(fd[1], "!", 1);
        TEST_ASSERT(ret == 1);

        while(1 == 1)
        {
            /* Just loop until the parent kills this process. */
        }
    }
    else if(pid > 0)
    {
        char *buf[1] = {0};

        /* Wait for a byte from the child saying it's safe to return. */
        ssize_t ret = read(fd[0], buf, 1);
        TEST_ASSERT(ret == 1);

        rtrn = kill(pid, SIGINT);
        TEST_ASSERT(rtrn == 0);

        TEST_ASSERT(state->children[0]->pid == EMPTY);

        (void)close(fd[0]);
        (void)close(fd[1]);

        return;
    }
    else
    {
        TEST_ABORT();
    }
}

static void test_kill_all_children(void)
{
    int32_t rtrn = 0;
    uint32_t total_children = 2;
    struct children_state *children_state = NULL;

    children_state = create_children_state(total_children);
    TEST_ASSERT_NOT_NULL(children_state);

    set_children_state(children_state);
    TEST_ASSERT_NOT_NULL(state);

    struct syscall_child *child_one = NULL;
    struct syscall_child *child_two = NULL;

    child_one = create_syscall_child();
    TEST_ASSERT_NOT_NULL(child_one);

    rtrn = child_one->start(child_one);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT(atomic_load_int32(&child_one->pid) > 0);
    TEST_ASSERT(children_state->running_children == 1);

    child_two = create_syscall_child();
    TEST_ASSERT_NOT_NULL(child_two);

    rtrn = child_two->start(child_two);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT(atomic_load_int32(&child_two->pid) > 0);
    TEST_ASSERT(children_state->running_children == 2);

    kill_all_children();

    TEST_ASSERT(state->children[0]->pid == EMPTY);
    TEST_ASSERT(state->children[1]->pid == EMPTY);

    return;
}

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

int main(void)
{
    setup_tests();
    test_set_children_state();
    test_create_children_state();
    test_create_syscall_child();
    test_get_child_with_pid();
    test_kill_all_children();
    test_setup_ctrlc_handler();
    return (0);
}

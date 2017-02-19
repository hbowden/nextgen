/**
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
 **/

#include "child.h"
#include "io/io.h"
#include "signals.h"
#include "syscall.h"
#include "utils/noreturn.h"
#include "memory/memory.h"
#include "concurrent/concurrent.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

static struct memory_allocator *allocator;
static struct output_writter *output;
static struct children_state *state = NULL;

static int32_t child_loop(struct syscall_child *child)
{
    (void)child;

    setup_child_signal_handler();

    struct test_case *test = NULL;

    while(1)
    {
        test = create_test_case();
        if(test == NULL)
        {
            output->write(ERROR, "Failed to create test case\n");
            return (-1);
        }

        cleanup_test(test);
    }
}

static int32_t start_child(struct syscall_child *child)
{
    pid_t pid = 0;
    int32_t fd[2];

    int32_t rtrn = pipe(fd);
    if(rtrn < 0)
    {
        output->write(ERROR, "Message pipe creation failed\n");
        return (-1);
    }

    pid = fork();
    if(pid == 0)
    {
        printf("id: %d\n", getpid());

        atomic_store_int32(&child->pid, getpid());
        atomic_add_uint32(&state->running_children, 1);

        /* Let the parent process know it's safe to continue. */
        ssize_t ret = write(fd[1], "!", 1);
        if(ret < 1)
        {
            output->write(ERROR, "Write: %s\n", strerror(errno));
            return (-1);
        }

        child_loop(child);

        _exit(0);
    }
    else if(pid > 0)
    {
        char *buf[1] = {0};

        /* Wait for a byte from the child saying it's safe to return. */
        ssize_t ret = read(fd[0], buf, 1);
        if(ret < 1)
        {
            output->write(ERROR, "Read: %s\n", strerror(errno));
            return (-1);
        }

        /* Clean up pipe descriptors and return. */
        (void)close(fd[0]);
        (void)close(fd[1]);
        return (0);
    }
    else
    {
        output->write(ERROR, "Failed to create child process: %s\n", strerror(errno));
        return (-1);
    }
}

void kill_all_children(void)
{
    uint32_t i;

    for(i = 0; i < state->total_children; i++)
    {
        pid_t pid = atomic_load_int32(&state->children[i]->pid);

        /* Ignore errors because we are going to be potentially looping over invalid pids. */
        kill(pid, SIGKILL);

        atomic_store_int32(&state->children[i]->pid, EMPTY);
    }

    return;
}

struct syscall_child *create_syscall_child(void)
{
    uint32_t i;

    for(i = 0; i < state->total_children; i++)
    {
        struct syscall_child *child = state->children[i];

        if(atomic_load_int32(&child->pid) == EMPTY)
        {
            /* Try setting this child object to INITIALIZING so other threads won't try and change it. */
            if(ck_pr_cas_int(&child->pid, EMPTY, INITIALIZING) == true)
                return (child);
        }
    }

    return (NULL);
}

struct children_state *create_children_state(uint32_t total_children)
{
    struct children_state *child_state = NULL;
    struct children_state tmp_state = {
      .total_children = total_children,
      .test_counter = 0,
      .running_children = 0
    };

    child_state = allocator->shared(sizeof(struct children_state));
    if(child_state == NULL)
    {
        output->write(ERROR, "child_state object allocation failed\n");
        return (NULL);
    }

    memmove(child_state, &tmp_state, sizeof(struct children_state));

    child_state->children = allocator->shared(sizeof(struct syscall_child *) * total_children);
    if(child_state->children == NULL)
    {
        output->write(ERROR, "child_state child array allocation failed\n");
        return (NULL);
    }

    uint32_t i;

    for(i = 0; i < total_children; i++)
    {

        child_state->children[i] = allocator->shared(sizeof(struct syscall_child));
        if(child_state->children[i] == NULL)
        {
            output->write(ERROR, "Child allocation failed\n");
            return (NULL);
        }

        child_state->children[i]->start = &start_child;
    }

    return (child_state);
}

struct syscall_child *get_child_with_pid(pid_t pid)
{
    uint32_t i;

    for(i = 0; i < state->total_children; i++)
    {
        if(atomic_load_ptr(&state->children[i]->pid) == pid)
            return (atomic_load_ptr(&state->children[i]));
    }

    /* Should not get here, but if we do return NULL. */
    return (NULL);
}

void set_children_state(struct children_state *c_state)
{
    state = c_state;

    return;
}

void set_had_error(struct syscall_child *child, int32_t val)
{
    atomic_store_int32(&child->had_error, val);
}

void set_ret_value(struct syscall_child *child, int32_t val)
{
    atomic_store_int32(&child->ret_value, val);
}

void set_sig_num(struct syscall_child *child, int32_t num)
{
    atomic_store_int32(&child->sig_num, num);
}

void set_did_jump(struct syscall_child *child, int32_t val)
{
    atomic_store_int32(&child->did_jump, val);
}

NX_NO_RETURN void jump(struct syscall_child *child)
{
    /* Jump to the return point saved earlier.
      No need for a return because it will not be executed. */
    longjmp(child->return_jump, 0);
}

void set_child_pid(struct syscall_child *child, int32_t pid)
{
    cas_loop_int32(&child->pid, pid);

    return;
}

void set_arg_size(struct syscall_child *child, uint64_t size)
{
    (void)child;
    (void)size;
}

void inject_child_deps(struct dependency_context *ctx)
{
    uint32_t i;

    for(i = 0; i < ctx->count; i++)
    {
        switch((int32_t)ctx->array[i]->name)
        {
            case ALLOCATOR:
                allocator = (struct memory_allocator *)ctx->array[i]->interface;
                break;

            case OUTPUT:
                output = (struct output_writter *)ctx->array[i]->interface;
                break;
        }
    }
}

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
#include "concurrent/concurrent.h"
#include <stdio.h>
#include <stdint.h>

// static struct children_state *state;

// static int32_t start_child(void)
// {
//     return (0);
// }
//
// static int32_t setup_child(void)
// {
//     (void)state;
//     return (0);
// }

int32_t create_syscall_child(struct output_writter *output,
                             struct children_state *child_state)
{
    uint32_t i;

    (void)output;

    for(i = 0; i < child_state->total_children; i++)
    {
        struct syscall_child *child = child_state->children[i];

        if(atomic_load_int32(&child->pid) == EMPTY)
        {
            /* Try setting this child object to INITIALIZING so other threads won't try and change it. */
            if(ck_pr_cas_int(&child->pid, EMPTY, INITIALIZING) == true)
                break;
        }
    }

    return (0);
}

struct children_state *create_children_state(struct memory_allocator *allocator,
                                             struct output_writter *output,
                                             uint32_t total_children)
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
        output->write(ERROR, "child_state child state object allocation failed\n");
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
    }

    return (child_state);
}

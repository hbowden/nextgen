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

#include "fuzzer.h"
#include "syscall/child.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

static int32_t stop_syscall_fuzzer(void)
{


    return (0);
}

static int32_t start_syscall_fuzzer(struct output_writter *output, struct memory_allocator *allocator)
{
    int32_t rtrn = 0;
    uint32_t core_count = 0;
    struct children_state *state = NULL;

    rtrn = get_core_count(&core_count);
    if(rtrn < 0)
    {
        output->write(ERROR, "Failed to get core count\n");
        return (-1);
    }

    state = create_children_state(allocator, output, core_count);
    if(state == NULL)
    {
        output->write(ERROR, "Can't create children state\n");
        return (-1);
    }

    while(1)
    {
        if(state->running_children < state->total_children)
        {
            struct syscall_child *child = NULL;
            child = create_syscall_child(state);
            if(child == NULL)
               continue;
        }
    }
}

static int32_t setup_syscall_fuzzer(void)
{

    return (0);
}

struct fuzzer_instance *get_syscall_fuzzer(char *output_path,
                                           struct memory_allocator *allocator,
                                           struct output_writter *output)
{
    if(output_path == NULL)
    {
        output->write(ERROR, "Output path is NULL\n");
        return (NULL);
    }

    struct fuzzer_instance *fuzzer = NULL;

    fuzzer = allocator->alloc(sizeof(struct fuzzer_instance));
    if(fuzzer == NULL)
    {
        output->write(ERROR, "Failed to allocate syscall fuzzer: %s\n", strerror(errno));
        return (NULL);
    }

    fuzzer->setup = &setup_syscall_fuzzer;
    fuzzer->start = &start_syscall_fuzzer;
    fuzzer->stop = &stop_syscall_fuzzer;

    return (fuzzer);
}

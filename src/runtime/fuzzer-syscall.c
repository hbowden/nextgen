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
#include "io/io.h"
#include "syscall/child.h"
#include "memory/memory.h"
#include "syscall/signals.h"
#include "utils/utils.h"
#include "platform.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

static struct output_writter *output;
static struct memory_allocator *allocator;
static struct fuzzer_control *control;

static int32_t stop_syscall_fuzzer(void)
{


    return (0);
}

static int32_t start_syscall_fuzzer(void)
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

    /* Set the max number of children processes to create as the number of
      processors(CPUS) present on the system we are running on.
      This currently counts intel hyper threading and other similar
      technologies as it's own core. */
    state = create_children_state(core_count);
    if(state == NULL)
    {
        output->write(ERROR, "Can't create children state\n");
        return (-1);
    }

    set_children_state(state);

    /* Setup our own signal handler for SIGINT or ctrl-c so we can
       cleanup the child processes before this, the main process exits.   */
    setup_ctrlc_handler();

    while(control->stop != TRUE)
    {
        if(state->running_children < state->total_children)
        {
            struct syscall_child *child = NULL;

            child = create_syscall_child();
            if(child == NULL)
               continue;

            rtrn = child->start(child);
            if(rtrn < 0)
            {
                output->write(ERROR, "Child process failed to start\n");
                return (-1);
            }
        }
    }

    return (0);
}

static int32_t setup_syscall_fuzzer(void)
{

    return (0);
}

struct fuzzer_instance *get_syscall_fuzzer(char *output_path)
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

void inject_syscall_fuzzer_deps(struct dependency_context *ctx)
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

            case CONTROL:
                control = (struct fuzzer_control *)ctx->array[i]->interface;
                break;
        }
    }
}

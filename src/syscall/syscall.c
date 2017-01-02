/**
 * Copyright (c) 2015, Harrison Bowden, Minneapolis, MN
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

#include "syscall.h"
#include "arg_types.h"
#include "child.h"
#include "signals.h"
#include "generate.h"
#include "memory/memory.h"
#include "io/io.h"

static struct memory_allocator *allocator;
static struct output_writter *output;

struct test_case
{
    int value;
};

struct test_case *create_test_case(void)
{
    struct test_case *test = NULL;

    test = allocator->alloc(sizeof(struct test_case));
    if(test == NULL)
    {
        output->write(ERROR, "Failed to allocate test case\n");
        return (NULL);
    }

    return (test);
}

void inject_syscall_deps(struct dependency_context *ctx)
{
    inject_arg_types_deps(ctx);
    inject_child_deps(ctx);
    inject_signal_deps(ctx);
    inject_generate_deps(ctx);

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

    return;
}

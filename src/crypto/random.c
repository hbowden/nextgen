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

#include "random.h"
#include "memory/memory.h"
#include "io/io.h"
#include <stdlib.h>

static struct memory_allocator *allocator;
static struct output_writter *output;

static int32_t default_rand_bytes(char **buf, uint32_t length)
{
    (*buf) = allocator->alloc(length + 1);
    if((*buf) == NULL)
    {
        output->write(ERROR, "Buffer allocation failed\n");
        return (-1);
    }

    arc4random_buf((*buf), length);

    /* null terminate the string. */
    (*buf)[length] = '\0';

    return (0);
}

static int32_t default_rand_range(uint32_t range, uint32_t *number)
{
    (*number) = arc4random_uniform(range);

    return (0);
}

struct random_generator *get_default_random_generator(void)
{
    struct random_generator *random = NULL;

    random = allocator->alloc(sizeof(struct random_generator));
    if(random == NULL)
    {
        output->write(ERROR, "Random generator allocation failed");
        return (NULL);
    }

    random->range = &default_rand_range;
    random->bytes = &default_rand_bytes;

    return (random);
}

void inject_random_deps(struct dependency_context *ctx)
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

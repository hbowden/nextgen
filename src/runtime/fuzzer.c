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

static struct output_writter *output;
static struct memory_allocator *allocator;

struct fuzzer_instance *get_fuzzer(struct fuzzer_config *config)
{
    if(config == NULL)
    {
        output->write(ERROR, "Config is NULL\n");
        return (NULL);
    }

    switch((int32_t)config->mode)
    {
        case MODE_SYSCALL:
            return(get_syscall_fuzzer(config->output_path));

        default:
            output->write(ERROR, "No fuzzer type selected\n");
            return (NULL);
    }
}

struct fuzzer_control *init_fuzzer_control(void)
{
    struct fuzzer_control *control = NULL;

    control = allocator->alloc(sizeof(struct fuzzer_control));
    if(control == NULL)
    {
        output->write(ERROR, "Failed to allocate fuzzer control\n");
        return (NULL);
    }

    control->stop = FALSE;

    return (control);
}

void inject_fuzzer_deps(struct dependency_context *ctx)
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

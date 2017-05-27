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

#include "arg_types.h"
#include "io/io.h"
#include "log/log.h"

static struct output_writter *output;

struct arg_context address_ctx = {
    .name = "ADDRESS",
    .type = ADDRESS,
    .should_free = NX_YES
};

struct arg_context int_ctx = {
    .name = "INT",
    .type = INT,
    .should_free = NX_NO
};

struct arg_context pid_ctx = {
    .name = "PID",
    .type = PID,
    .should_free = NX_NO
};


void inject_arg_types_deps(struct dependency_context *ctx)
{
    uint32_t i;

    for(i = 0; i < ctx->count; i++)
    {
        switch((int32_t)ctx->array[i]->name)
        {
            case OUTPUT:

                output = (struct output_writter *)ctx->array[i]->interface;
                break;
        }
    }
}

struct arg_context *get_arg_context(enum arg_type type)
{
    switch((int32_t)type)
    {
        case ADDRESS:
            return &address_ctx;

        case INT:
            return &int_ctx;

        case PID:
            return &pid_ctx;

        default:
            output->write(ERROR, "Unlisted arg type\n");
            return (NULL);
    }
}

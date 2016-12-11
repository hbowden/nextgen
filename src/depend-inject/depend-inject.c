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

#include "depend-inject.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

struct dependency_context *create_dependency_ctx(struct dependency *dependency, ...)
{
    va_list args;
    uint32_t count = 1;
    va_start(args, dependency);
    struct dependency *dep = NULL;
    struct dependency **array = NULL;
    struct dependency_context *ctx = NULL;

    ctx = malloc(sizeof(struct dependency_context));
    if(ctx == NULL)
    {
        printf("Malloc: %s\n", strerror(errno));
        return (NULL);
    }

    for(dep = dependency; dep != NULL; dep = va_arg(args, struct dependency *))
    {
        array = realloc(array, sizeof(struct dependency *) * count);
        if(array == NULL)
        {
            printf("Malloc: %s\n", strerror(errno));
            return (NULL);
        }

        array[count - 1] = dep;
        count++;
    }

    ctx->count = count - 1;
    ctx->array = array;

    return (ctx);
}

struct dependency *create_dependency(void *interface, enum dependency_name name)
{
    struct dependency *dep = NULL;

    dep = malloc(sizeof(struct dependency));
    if(dep == NULL)
    {
        printf("Malloc: %s\n", strerror(errno));
        return (NULL);
    }

    dep->name = name;
    dep->interface = interface;

    return (dep);
}

int32_t add_dep(struct dependency_context *ctx, struct dependency *dep)
{
    ctx->count = ctx->count + 1;

    ctx->array = realloc(ctx->array, sizeof(struct dependency *) * ctx->count);
    if(ctx->array == NULL)
    {
        printf("Malloc: %s\n", strerror(errno));
        return (-1);
    }

    ctx->array[ctx->count - 1] = dep;

    return (0);
}

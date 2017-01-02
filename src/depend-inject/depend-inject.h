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

#ifndef NX_DEPEND_INJECT_H
#define NX_DEPEND_INJECT_H

#include <stdint.h>

enum dependency_name {OUTPUT, ALLOCATOR, RANDOM_GEN, HASHER, CONTROL, RESOURCE_GEN};

struct dependency
{
    void *interface;
    enum dependency_name name;
};

struct dependency_context
{
    struct dependency **array;
    uint32_t count;
};

extern struct dependency_context *create_dependency_ctx(struct dependency *, ...);

extern struct dependency *create_dependency(void *interface, enum dependency_name name);

extern int32_t add_dep(struct dependency_context *, struct dependency *);

#endif

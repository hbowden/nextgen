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

#include "runtime.h"
#include "platform.h"
#include "crypto/crypto.h"
#include "depend-inject/depend-inject.h"

void inject_deps(struct output_writter *output,
                 struct memory_allocator *allocator)
{
    struct dependency_context *ctx = NULL;

    ctx = create_dependency_ctx(create_dependency(output, OUTPUT),
                                create_dependency(allocator, ALLOCATOR),
                                NULL);

    inject_crypto_deps(ctx);


    return;
}

const char *get_os(void)
{
    return (OPERATING_SYSTEM);
}

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
#include "fuzzer.h"
#include "platform.h"
#include "crypto/crypto.h"
#include "crypto/random.h"
#include "crypto/hash.h"
#include "utils/utils.h"
#include "resource/resource.h"
#include "depend-inject/depend-inject.h"

static int32_t verbose;

int32_t get_verbosity(void)
{
    return (verbose);
}

void set_verbosity(int32_t v)
{
    verbose = v;
}

static int32_t inject_runtime_deps(struct dependency_context *ctx)
{
    inject_fuzzer_deps(ctx);
    inject_syscall_fuzzer_deps(ctx);
    inject_nextgen_deps(ctx);

    return (0);
}

int32_t inject_deps(struct output_writter *output)
{
    struct memory_allocator *allocator = NULL;

    allocator = get_default_allocator();
    if(allocator == NULL)
    {
        output->write(ERROR, "Failed to get memory allocator\n");
        return (-1);
    }

    struct dependency_context *ctx = NULL;

    ctx = create_dependency_ctx(create_dependency(output, OUTPUT),
                                create_dependency(allocator, ALLOCATOR),
                                NULL);
    if(ctx == NULL)
    {
        output->write(ERROR, "Failed to create dependency context\n");
        return (-1);
    }

    inject_crypto_deps(ctx);

    struct random_generator *random_gen = NULL;
    struct hasher *hasher = NULL;

    random_gen = get_default_random_generator();
    if(random_gen == NULL)
    {
        output->write(ERROR, "Can't get random generator\n");
        return (-1);
    }

    hasher = get_hasher();
    if(hasher == NULL)
    {
        output->write(ERROR, "Failed to get hasher\n");
        return (-1);
    }

    add_dep(ctx, create_dependency(hasher, HASHER));
    add_dep(ctx, create_dependency(random_gen, RANDOM_GEN));

    inject_utils_deps(ctx);
    inject_resource_deps(ctx);
    inject_runtime_deps(ctx);

    struct fuzzer_control *control = NULL;

    control = init_fuzzer_control();
    if(control == NULL)
    {
        output->write(ERROR, "Fuzzer control object initialation failed\n");
        return (-1);
    }

    add_dep(ctx, create_dependency(control, CONTROL));

    /* A hack, there's definetly a better way. Currently init_fuzzer_control()
       needs the allocator interface injected into runtime. But runtime also needs control injected
       as well hence the double inject. */
    inject_runtime_deps(ctx);

    return (0);
}

const char *get_os(void)
{
    return (OPERATING_SYSTEM);
}

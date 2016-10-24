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
#include <stdio.h>
#include <errno.h>
#include <string.h>

static int32_t stop_syscall_fuzzer(void)
{


    return (0);
}

static int32_t start_syscall_fuzzer(void)
{


    return (0);
}

static int32_t setup_syscall_fuzzer(struct random_generator *random,
                                    struct output_writter *output)
{
    int32_t rtrn = 0;
    rtrn = random->seed();
    if(rtrn < 0)
    {
        output->write(ERROR, "Failed to seed random number generator\n");
        return (-1);
    }

    return (0);
}

struct fuzzer_instance *get_syscall_fuzzer(struct memory_allocator *allocator,
                                           struct output_writter *output)
{
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

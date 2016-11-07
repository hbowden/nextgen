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

struct fuzzer_instance *get_fuzzer(struct fuzzer_config *config,
                                   struct memory_allocator *allocator,
                                   struct output_writter *output)
{
    if(config == NULL)
    {
        output->write(ERROR, "Config is NULL\n");
        return (NULL);
    }

    switch((int32_t)config->mode)
    {
        case MODE_SYSCALL:
            return(get_syscall_fuzzer(config->output_path, allocator, output));

        default:
            output->write(ERROR, "No fuzzer type selected\n");
            return (NULL);
    }
}

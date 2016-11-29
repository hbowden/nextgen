

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

#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include "io/io.h"
#include "utils/deprecate.h"
#include "memory/memory.h"

struct random_generator
{
    int32_t (*range)(uint32_t, uint32_t *);
    int32_t (*bytes)(struct memory_allocator *, struct output_writter *, char **, uint32_t);
};

/* Fill the buffer out with the sha256 hash of in. */
DEPRECATED extern int32_t sha256(char *in, char **out);

/* Fill the buffer out with the sha512 hash of in. */
DEPRECATED extern int32_t sha512(char *in, char **out);

extern struct random_generator *get_default_random_generator(struct memory_allocator *,
                                                             struct output_writter *);

#endif /* End of header file. */

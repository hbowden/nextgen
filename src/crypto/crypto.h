

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

/* Pass either CRYPTO or NO_CRYPTO to setup_crypto_module().
   When NO_CRYPTO is passed then the crypto module use's non
   cryptographic random number generators.  */
enum crypto_method {CRYPTO, NO_CRYPTO};

struct random_generator
{
    int32_t (*range)(uint32_t, uint32_t *);
    int32_t (*bytes)(char **, uint32_t);
    int32_t (*seed)(void);
};

/* Call this once to setup the crypto module. */
DEPRECATED extern int32_t setup_crypto_module(enum crypto_method method);

/* Use this function to see if were using a software PRNG or hardware.
   This function is useful for knowing when we need to seed the PRNG on forks. */
DEPRECATED extern int32_t using_hardware_prng(void);

/* This function seeds the software PRNG. */
DEPRECATED extern int32_t seed_prng(void);

/* Create a random number in the range set by the variable range.  */
DEPRECATED extern int32_t rand_range(uint32_t range, uint32_t *number);

/* Fill the buffer with length random bytes. */
DEPRECATED extern int32_t rand_bytes(char **buf, uint32_t length);

/* Fill the buffer out with the sha256 hash of in. */
DEPRECATED extern int32_t sha256(char *in, char **out);

/* Fill the buffer out with the sha512 hash of in. */
DEPRECATED extern int32_t sha512(char *in, char **out);

extern struct random_generator *get_default_random_generator(struct memory_allocator *,
                                                             struct output_writter *);

#endif /* End of header file. */

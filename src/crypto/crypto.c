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

#include "crypto.h"
#include "io/io.h"
#include "memory/memory.h"
#include "openssl/bn.h"
#include "openssl/crypto.h"
#include "openssl/engine.h"
#include "openssl/evp.h"
#include "openssl/rand.h"
#include "openssl/sha.h"
#include "utils/autofree.h"
#include "utils/autoclose.h"
#include "runtime/platform.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static int32_t default_rand_bytes(struct memory_allocator *allocator,
                                  struct output_writter *output,
                                  char **buf,
                                  uint32_t length)
{
    (*buf) = allocator->alloc(length + 1);
    if((*buf) == NULL)
    {
        output->write(ERROR, "Buffer allocation failed\n");
        return (-1);
    }

    int32_t rtrn = RAND_bytes((unsigned char *)*buf, (int32_t)length);
    if(rtrn != 1)
    {
        output->write(ERROR, "Can't get random bytes\n");
        return (-1);
    }

    /* null terminate the string. */
    (*buf)[length] = '\0';

    return (0);
}

static int32_t default_rand_range(uint32_t range, uint32_t *number)
{
    int32_t rtrn = 0;
    BIGNUM *random, *range1;

    random = BN_new();
    if(random == NULL)
    {
        printf("Can't init bignum struct\n");
        return (-1);
    }

    range1 = BN_new();
    if(range1 == NULL)
    {
        printf("Can't init bignum struct\n");
        return (-1);
    }

    rtrn = BN_set_word(range1, (range + 1));
    if(rtrn < 0)
    {
        printf("Can't set range\n");
        return (-1);
    }

    rtrn = BN_rand_range(random, range1);
    if(rtrn < 0)
    {
        printf("Can't get random range\n");
        return -1;
    }

    char *buf = BN_bn2dec(random);
    if(buf == NULL)
    {
        printf("Can't convert random number\n");
        return (-1);
    }

    (*number) = (uint32_t)strtol(buf, NULL, 10);

    return (0);
}

struct random_generator *get_default_random_generator(struct memory_allocator *allocator,
                                                      struct output_writter *output)
{
    struct random_generator *random = NULL;

    random = allocator->alloc(sizeof(struct random_generator));
    if(random == NULL)
    {
        output->write(ERROR, "Random generator allocation failed");
        return (NULL);
    }

    random->range = &default_rand_range;
    random->bytes = &default_rand_bytes;

    return (random);
}

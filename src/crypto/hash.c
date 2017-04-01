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

#include "hash.h"
#include "memory/memory.h"
#include "openssl/sha.h"
#include "io/io.h"
#include <string.h>
#include <stdio.h>

static struct memory_allocator *allocator;
static struct output_writter *output;

static int32_t sha512(char *in, char **out)
{
    SHA512_CTX ctx;
    uint32_t i = 0;
    int32_t rtrn = 0;
    unsigned char hash[SHA512_DIGEST_LENGTH];

    rtrn = SHA512_Init(&ctx);
    if(rtrn < 0)
    {
        output->write(ERROR,"Sha init\n");
        return (-1);
    }

    rtrn = SHA512_Update(&ctx, in, strlen(in));
    if(rtrn < 0)
    {
        output->write(ERROR,"Can't update input string\n");
        return (-1);
    }

    rtrn = SHA512_Final(hash, &ctx);
    if(rtrn < 0)
    {
        output->write(ERROR,"Can't do this\n");
        return (-1);
    }

    (*out) = allocator->alloc((SHA512_DIGEST_LENGTH * 2) + 1);
    if((*out) == NULL)
    {
        output->write(ERROR,"Can't allocate output buf\n");
        return (-1);
    }

    for(i = 0; i < SHA512_DIGEST_LENGTH; i++)
        sprintf(*out + (i * 2), "%02x", hash[i]);

    (*out)[128] = '\0';

    return (0);
}

static int32_t sha256(char *in, char **out)
{
    /* Declarations */
    uint32_t i = 0;
    int32_t rtrn = 0;
    SHA256_CTX sha256;
    unsigned char hash[SHA256_DIGEST_LENGTH];

    rtrn = SHA256_Init(&sha256);
    if(rtrn < 0)
    {
        output->write(ERROR,"Sha Init Error\n");
        return (-1);
    }

    rtrn = SHA256_Update(&sha256, in, strlen(in));
    if(rtrn < 0)
    {
        output->write(ERROR,"Sha Update Error\n");
        return (-1);
    }

    rtrn = SHA256_Final(hash, &sha256);
    if(rtrn < 0)
    {
        output->write(ERROR,"Sha Final Error\n");
        return (-1);
    }

    /* Allocate the output buffer. */
    (*out) = allocator->alloc((SHA256_DIGEST_LENGTH * 2) + 1);
    if((*out) == NULL)
    {
        output->write(ERROR,"Can't allocate output buf\n");
        return (-1);
    }

    /* Convert hash to hex format. */
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(*out + (i * 2), "%02x", hash[i]);

    /* null terminate the hash. */
    (*out)[64] = '\0';

    return (0);
}

struct hasher *get_hasher(void)
{
    struct hasher *hasher = NULL;

    hasher = allocator->alloc(sizeof(struct hasher));
    if(hasher == NULL)
    {
        output->write(ERROR, "Failed to allocate hasher\n");
        return (NULL);
    }

    hasher->sha256 = &sha256;
    hasher->sha512 = &sha512;

    return (hasher);
}

void inject_hash_deps(struct dependency_context *ctx)
{
    uint32_t i;

    for(i = 0; i < ctx->count; i++)
    {
        switch((int32_t)ctx->array[i]->name)
        {
            case ALLOCATOR:
                allocator = (struct memory_allocator *)ctx->array[i]->interface;
                break;

            case OUTPUT:
                output = (struct output_writter *)ctx->array[i]->interface;
                break;
        }
    }
}

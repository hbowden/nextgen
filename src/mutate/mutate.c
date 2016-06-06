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

#include "mutate.h"
#include "crypto/crypto.h"
#include "io/io.h"
#include "plugins/plugin.h"
#include "runtime/platform.h"

#include <stdbool.h>

static int32_t flip_byte_mutator(char **file, uint64_t *file_size)
{
    int32_t rtrn = 0;
    uint32_t offset = 0;

    rtrn = rand_range((uint32_t)(*file_size), &offset);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random file offset\n");
        return (-1);
    }

    /* Flip the randomly choosen byte. */
    (*file)[offset] = ~(*file)[offset];

    return (0);
}

static int32_t flip_bit_mutator(char **file, uint64_t *file_size)
{
    int32_t rtrn = 0;
    uint32_t offset = 0;
    uint32_t bit = 0;

    rtrn = rand_range((uint32_t)(*file_size), &offset);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random file offset\n");
        return (-1);
    }

    rtrn = rand_range(8, &bit);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random bit to flip\n");
        return (-1);
    }

    /* Flip the randomly choosen bit. */
    (*file)[offset] ^= 1 << bit;

    return (0);
}

static int32_t xor_mutator(char **file, uint64_t *file_size) { return (0); }

static uint32_t number_of_mutators = 3;

/* Array of file mutator function pointers. */
static int32_t (*mutator_array[])(char **file, uint64_t *file_size) = {
    flip_byte_mutator, flip_bit_mutator, xor_mutator};

static int32_t mutate_file_randomly(char **file, uint64_t *file_size)
{
    int32_t rtrn = 0;
    uint32_t offset = 0;

    /* Pick a mutator function offset at random. */
    rtrn = rand_range((number_of_mutators - 1), &offset);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random offset\n");
        return (-1);
    }

    /* Call the mutator selected and return the result. */
    return (mutator_array[offset](file, file_size));
}

int32_t mutate_file(char **file, const char *file_extension,
                    uint64_t *file_size)
{
    int32_t rtrn = 0;
    bool understand_file_format = FALSE;
    uint32_t plugin_offset = 0;
    uint32_t number_of_features = 0;
    uint32_t feature = 0;
    uint32_t number = 0;

    /* Check if we understand the file format so we can provide context
    to the mutation engine. */
    if(supported_file(file_extension, &plugin_offset) == TRUE)
    {
        /* Some plugin understands this file format, let's use the plugin for more
        accurate fuzzing. Check how many features the plugin know's about. */
        features_supported(plugin_offset, &number_of_features);

        /* Sometimes randomly mutate the file instead of using the plugin. */
        rtrn = rand_range(10, &number);
        if(rtrn < 0)
        {
            output(ERROR, "Can't randomly select number\n");
            return (-1);
        }

        /* If the number is equal to ten mutate the file completly randomly. */
        if(number == 10)
        {
            rtrn = mutate_file_randomly(file, file_size);
            if(rtrn < 0)
            {
                output(ERROR, "Can't mutate file randomly\n");
                return (-1);
            }

            /* Exit early. */
            return (0);
        }

        /* Randomly select a file format feature to test. */
        rtrn = rand_range(number_of_features, &feature);
        if(rtrn < 0)
        {
            output(ERROR, "Can't randomly select feature to test\n");
            return (-1);
        }

        struct feature_constraints *constraints = NULL;

        /* Get feature constraints.*/
        rtrn = feature_constraints(plugin_offset, constraints);
        if(rtrn < 0)
        {
            output(ERROR, "Can't get feature constraints\n");
            return (-1);
        }
    }
    else
    {
        /* We don't know this file type, so let's just randomly mutate
        the file. */
        rtrn = mutate_file_randomly(file, file_size);
        if(rtrn < 0)
        {
            output(ERROR, "Can't mutate file randomly\n");
            return (-1);
        }
    }

    return (0);
}

int32_t mutate_arguments(uint64_t **args, uint64_t *size) { return (0); }

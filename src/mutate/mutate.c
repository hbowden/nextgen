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
#include "runtime/platform.h"

#include <stdbool.h>
#include <stdio.h>

// static int32_t flip_byte_mutator(char **file,
//                                  uint64_t *file_size,
//                                  struct random_generator *random,
//                                  struct output_writter *output)
// {
//     int32_t rtrn = 0;
//     uint32_t offset = 0;
//
//     rtrn = random->range((uint32_t)(*file_size), &offset);
//     if(rtrn < 0)
//     {
//         output->write(ERROR, "Can't pick random file offset\n");
//         return (-1);
//     }
//
//     /* Flip the randomly choosen byte. */
//     (*file)[offset] = ~(*file)[offset];
//
//     return (0);
// }
//
// static int32_t flip_bit_mutator(char **file,
//                                 uint64_t *file_size,
//                                 struct random_generator *random,
//                                 struct output_writter *output)
// {
//     int32_t rtrn = 0;
//     uint32_t offset = 0;
//     uint32_t bit = 0;
//
//     rtrn = random->range((uint32_t)(*file_size), &offset);
//     if(rtrn < 0)
//     {
//         output->write(ERROR, "Can't pick random file offset\n");
//         return (-1);
//     }
//
//     rtrn = random->range(8, &bit);
//     if(rtrn < 0)
//     {
//         output->write(ERROR, "Can't pick random bit to flip\n");
//         return (-1);
//     }
//
//     /* Flip the randomly choosen bit. */
//     (*file)[offset] ^= 1 << bit;
//
//     return (0);
// }
//
// static int32_t xor_mutator(char **file, uint64_t *file_size, struct random_generator *random, struct output_writter *output)
// {
//     (void)file;
//     (void)file_size;
//     (void)random;
//     (void)output;
//     return (0);
// }
//
// static uint32_t number_of_mutators = 3;
//
// /* Array of file mutator function pointers. */
// static int32_t (*mutator_array[])(char **, uint64_t *, struct random_generator *, struct output_writter *) = {
//     flip_byte_mutator, flip_bit_mutator, xor_mutator};

// static int32_t mutate_file_randomly(char **file, uint64_t *file_size, struct random_generator *random, struct output_writter *output)
// {
//     int32_t rtrn = 0;
//     uint32_t offset = 0;
//
//     /* Pick a mutator function offset at random. */
//     rtrn = random->range((number_of_mutators - 1), &offset);
//     if(rtrn < 0)
//     {
//         output->write(ERROR, "Can't pick random offset\n");
//         return (-1);
//     }
//
//     /* Call the mutator selected and return the result. */
//     return (mutator_array[offset](file, file_size, random, output));
// }

// int32_t mutate_file(void)
// {
//
//
//     return (0);
// }
//
// int32_t mutate_arguments(uint64_t **args, uint64_t *size, struct random_generator *random, struct output_writter *output)
// {
//     (void)args;
//     (void)size;
//     (void)random;
//     (void)output;
//     return (0);
// }

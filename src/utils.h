

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

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>

#include "stdatomic.h"

enum yes_no { YES, NO };

enum csp { SET, UNSET };

enum name_type { DIR_NAME, FILE_NAME };

/* This function as the name implies gets the file size. */
extern int32_t get_file_size(int32_t fd, uint64_t *size);

/* Get the file extension */
extern int32_t get_extension(char *path, char **extension);

/* Get the core count of the system we are on. This will include virtual cores on hyperthreaded systems. */
extern int32_t get_core_count(uint32_t *core_count);

/* Can be used to create random file and directory na mes. */
extern int32_t generate_name(char **name, char *extension, enum name_type type);

/* Grabs the path to the user's home directory. */
extern int32_t get_home(char **home);

/* Convert the ascii string to a binary string upto len bytes length. */
extern int32_t ascii_to_binary(char *input, char **out, uint64_t len);

/* This function will return zero if the process calling it has root privileges. */
extern int32_t check_root(void);

/* Implementation of itoa. */
extern int32_t itoa(int32_t value, char *sp, int32_t radix);

#endif

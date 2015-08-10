

/**
 * Copyright (c) 2015, Harrison Bowden, Secure Labs, Minneapolis, MN
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

#ifndef ENTRY_H
#define ENTRY_H

#include "nextgen.h"

#include <stdbool.h>
#include <stdatomic.h>

enum syscall_status { ON, OFF };

enum alarm_status { YES, NO };

/* In memory format. */
struct syscall_entry_shadow
{
    const char *name_of_syscall;

    atomic_bool status;
    const bool need_alarm;
    const bool requires_root;

    const unsigned int number_of_args;
    const unsigned int entry_number;

    int arg_type_index[7];
    int (*get_arg_index[7])(unsigned long *);

    atomic_uint_least64_t return_value;
};

/* The on disk format. */
struct syscall_entry
{
    const char *name_of_syscall;

    bool status;
    const bool need_alarm;
    const bool requires_root;

    const unsigned int number_of_args;
    const unsigned int entry_number;

    int arg_type_index[7];
    int (*get_arg_index[7])(unsigned long *);
};

#endif

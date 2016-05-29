

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

#ifndef ENTRY_H
#define ENTRY_H

#include "context.h"
#include "runtime/platform.h"
#include "arg_types.h"
#include "stdatomic.h"

#include <stdbool.h>

enum test_id { V_ID, P_ID, VP_ID, VPV_ID, PV_ID };

enum syscall_status { ON, OFF };

enum argnums { FIRST_ARG, SECOND_ARG, THIRD_ARG, FOURTH_ARG, FIFTH_ARG, SIXTH_ARG};

/* In memory format. */
struct syscall_entry_shadow
{
    const char *syscall_name;
    const int32_t syscall_symbol;

    atomic_bool status;
    const bool need_alarm;
    const bool requires_root;

    const char padding[1];

    const uint32_t total_args;
    const uint32_t entry_number;

    struct arg_context *arg_context_array[7];

    int32_t (*get_arg_array[7])(uint64_t **, struct child_ctx *);

    atomic_uint_least64_t return_value;

    int32_t (*test_syscall)(int32_t, uint64_t **);
};

/* The on disk format. */
struct syscall_entry
{
    const char *syscall_name;
    const int32_t syscall_symbol;

    bool status;
    const bool need_alarm;
    const bool requires_root;

    const char padding[1];
    int32_t arg_type_array[7];

    const uint32_t total_args;
    const uint32_t entry_number;

    const char padding2[4];

    int32_t (*get_arg_array[7])(uint64_t **, struct child_ctx *);

    enum test_id id;
};

#endif

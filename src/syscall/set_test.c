

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

#include "set_test.h"
#include "log/log.h"
#include "utils/utils.h"
#include <unistd.h>

static int32_t test_val_ptr_val_ptr(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, (*array[0]), array[1], (*array[2]), array[3]));
}

static int32_t test_val_ptr_val_val_ptr_val(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol,
                    (*array[0]),
                    array[1],
                    (*array[2]),
                    (*array[3]),
                    array[4],
                    (*array[5])));
}

static int32_t test_val_val_ptr_val(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol,
                    (*array[0]),
                    (*array[1]),
                    array[2],
                    (*array[3])));
}

static int32_t test_ptr_val_ptr(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, array[0], (*array[1]), array[2]));
}

static int32_t test_ptr_ptr_val(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, array[0], array[1], (*array[2])));
}

static int32_t test_ptr_val_val(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, (*array[0]), (*array[1])));
}

static int32_t test_ptr_ptr_ptr(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, array[0], array[1], array[2]));
}

static int32_t test_val_ptr_ptr(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, (*array[0]), array[1], array[2]));
}

static int32_t test_val_val_ptr(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, (*array[0]), (*array[1]), array[2]));
}

static int32_t test_val_ptr_val(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, (*array[0]), array[1], (*array[2])));
}

static int32_t test_val_val_val(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, (*array[0]), (*array[1]), (*array[2])));
}

static int32_t test_val_val(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, (*array[0]), (*array[1])));
}

static int32_t test_val_ptr(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, (*array[0]), array[1]));
}

static int32_t test_ptr_val(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, array[0], (*array[1])));
}

static int32_t test_ptr_ptr(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, array[0], array[1]));
}

static int32_t test_ptr(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, array[0]));
}

static int32_t test_val(int32_t symbol, uint64_t **array)
{
    return (syscall(symbol, (*array[0])));
}

void set_test_syscall(struct syscall_entry *entry, enum test_id id)
{
    switch(id)
    {
        case V_ID:
            entry->test_syscall = &test_val;
            break;

        case P_ID:
            entry->test_syscall = &test_ptr;
            break;

        case PV_ID:
            entry->test_syscall = &test_ptr_val;
            break;

        case VP_ID:
            entry->test_syscall = &test_val_ptr;
            break;

        case VPV_ID:
            entry->test_syscall = &test_val_ptr_val;
            break;

        case PVV_ID:
            entry->test_syscall = &test_ptr_val_val;
            break;

        case PP_ID:
            entry->test_syscall = &test_ptr_ptr;
            break;

        case VVPV_ID:
            entry->test_syscall = &test_val_val_ptr_val;
            break;

        case VPVVPV_ID:
            entry->test_syscall = &test_val_ptr_val_val_ptr_val;
            break;

        case VPVP_ID:
            entry->test_syscall = &test_val_ptr_val_ptr;
            break;
    }

    return;
}

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

#include "generate.h"
#include "io/io.h"
#include "memory/memory.h"
#include "crypto/crypto.h"

#include <string.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/ptrace.h>

int32_t generate_mount_flags(uint64_t **flag, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t range = 7;
    uint32_t number = 0;
    int32_t flags[] = {MNT_RDONLY, MNT_NOEXEC,      MNT_NOSUID,  MNT_NODEV,
                       MNT_UNION,  MNT_SYNCHRONOUS, MNT_CPROTECT};

    (*flag) = mem_alloc(sizeof(int32_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc mount flags\n");
        return (-1);
    }

    rtrn = rand_range(range, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    /* Copy randomly chosen flag value to flag buffer. */
    memcpy((*flag), &flags[number], sizeof(int32_t));

    /* Set argument size. */
    set_arg_size(child, sizeof(int32_t));

    return (0);
}

int32_t generate_unmount_flags(uint64_t **flag, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t range = 7;
    uint32_t number = 0;

    int32_t flags[] = {MNT_RDONLY, MNT_NOEXEC, MNT_NOSUID, MNT_NODEV,
                       MNT_UNION, MNT_SYNCHRONOUS, MNT_CPROTECT};

    (*flag) = mem_alloc(sizeof(uint64_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc mount flags\n");
        return (-1);
    }

    rtrn = rand_range(range, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    /* Copy randomly chosen flag value to flag buffer. */
    memcpy((*flag), &flags[number], sizeof(flags[number]));

    /* Set arg size. */
    set_arg_size(child, sizeof(int32_t));

    return (0);
}

int32_t generate_request(uint64_t **flag, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t range = 7;
    uint32_t number = 0;
    int32_t request[] = {PT_TRACE_ME, PT_DENY_ATTACH, PT_CONTINUE,  PT_STEP,
                         PT_KILL,     PT_ATTACH,      PT_ATTACHEXC, PT_DETACH};

    (*flag) = mem_alloc(sizeof(int32_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc mount flags\n");
        return (-1);
    }

    rtrn = rand_range(range, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    memcpy((*flag), &request[number], sizeof(int32_t));

    set_arg_size(child, sizeof(int32_t));

    return (0);
}

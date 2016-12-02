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

#include "utils.h"
#include <sys/sysctl.h>
#include <errno.h>

int32_t get_core_count(uint32_t *core_count)
{
    int32_t mib[4];

    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;

    size_t len = sizeof(uint32_t);

    int32_t rtrn = sysctl(mib, 2, core_count, &len, NULL, 0);
    if(rtrn < 0)
    {
        printf("sysctl: %s\n", strerror(errno));
        return (-1);
    }

    if((*core_count) < 1)
        (*core_count) = (uint32_t)1;

    return (0);
}

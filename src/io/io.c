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

#include "io.h"
#include "utils/utils.h"
#include "runtime/platform.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

static void console_output(enum out_type type, const char *format, ...)
{
    va_list args;

    va_start(args, format);

    switch((int32_t)type)
    {
        case ERROR:
            vfprintf(stderr, format, args);
            break;

        case STD:
            vfprintf(stdout, format, args);
            break;

        /* We default to stdout when ERROR or STD is not set so that
        we don't have any errors. */
        default:
            vfprintf(stdout, format, args);
            break;
    }

    va_end(args);

    (void)fflush(stdout);

    return;
}

struct output_writter *get_console_writter(void)
{
    struct output_writter *console = NULL;

    console = malloc(sizeof(struct output_writter));
    if(console == NULL)
    {
        printf("Failed to allocate console writter\n");
        return (NULL);
    }

    console->write = &console_output;

    return (console);
}

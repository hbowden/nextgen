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

#ifndef NX_AUTO_CLOSE_H
#define NX_AUTO_CLOSE_H

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

static inline void close_fd(int *fd)
{
    if((*fd) == 0)
        return;

    close((*fd));
}

static inline void close_dir(DIR **dir)
{
    if((*dir) == NULL)
        return;

    closedir((*dir));
}

static inline void close_fp(FILE **fp)
{
    if((*fp) == NULL)
        return;

    fclose((*fp));
}

#define auto_close_fp __attribute__((cleanup (close_fp)))

#define auto_close __attribute__((cleanup (close_fd)))

#define auto_close_dir __attribute__((cleanup (close_dir)))

#endif

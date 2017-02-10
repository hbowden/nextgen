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

#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

#ifdef FREEBSD

/* Define TRUE and FALSE. */
enum local_bool { FALSE, TRUE };
static const uint32_t ARG_LIMIT = 9;
static const uint32_t ARG_BUF_LEN = 4096;
static const uint32_t POOL_SIZE = 1024;
static const char OPERATING_SYSTEM[] = "FREEBSD";

#endif /* End of FreeBSD. */

#ifdef MAC_OS

/* In this header TRUE and FALSE are defined. */
#include <mach/boolean.h>

static const uint32_t ARG_LIMIT = 9;
static const uint32_t ARG_BUF_LEN = 4096;
static const uint32_t POOL_SIZE = 1024;
static const char OPERATING_SYSTEM[] = "MACOS";

#endif /* End of MAC OSX. */

#ifdef LINUX

/* Define TRUE and FALSE. */
enum local_bool { FALSE, TRUE };

static const uint32_t ARG_LIMIT = 9;
static const uint32_t ARG_BUF_LEN = 4096;
static const uint32_t POOL_SIZE = 1024;
static const char OPERATING_SYSTEM[] = "LINUX";

#endif /* End of Linux. */

#ifdef CYGWIN

/* Define TRUE and FALSE. */
enum local_bool { FALSE, TRUE };

static const uint32_t ARG_LIMIT = 9;
static const uint32_t ARG_BUF_LEN = 4096;
static const uint32_t POOL_SIZE = 1024;
static const char OPERATING_SYSTEM[] = "CYGWIN";

#endif /* End of cygwin. */

#endif /* End of header file. */

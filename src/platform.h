

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

#ifdef FREEBSD

#define ARG_LIMIT 7

enum local_bool { FALSE, TRUE };

#endif /* End of FreeBSD. */

#ifdef MAC_OSX

#define ARG_LIMIT 7

#include <mach/boolean.h>

/* EL Capitan has strict limits on how many files one
  can create, so create much less resources on mac osx. */
#define POOL_SIZE 1024

#define ARG_BUF_LEN 4096

#else

#define POOL_SIZE 1024

#endif /* End of MAC OSX. */

#endif /* End of header file. */

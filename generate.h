

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

#ifndef GENERATE_H
#define GENERATE_H

#include "private.h"

private extern int generate_fd(unsigned long **fd);

private extern int generate_socket(unsigned long **sock);

private extern int generate_buf(unsigned long **buf);

private extern int generate_length(unsigned long **length);

private extern int generate_path(unsigned long **path);

private extern int generate_mode(unsigned long **mode);

private extern int generate_open_flag(unsigned long **flag);

private extern int generate_mode(unsigned long **mode);

private extern int generate_fs_stat(unsigned long **stat);

private extern int generate_fs_stat_flag(unsigned long **flag);

#endif
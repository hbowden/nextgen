

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

#ifndef RESOURCE_H
#define RESOURCE_H

#include "memory.h"
#include <stdint.h>

//enum resource_type = { desc_r, mount_r, dirpath_r, file_r, socket_r };

struct resource_ctx
{
    struct memory_block *m_blk;

    void *ptr;
};

extern int32_t get_socket(void);

extern int32_t free_socket(int32_t *sock_fd);

extern int32_t get_desc(void);

extern int32_t free_desc(int32_t *fd);

extern char *get_mountpath(void);

extern int32_t free_mountpath(char **path);

extern char *get_dirpath(void);

extern int32_t free_dirpath(char **path);

extern char *get_filepath(void);

extern int32_t free_filepath(char **path);

extern int32_t setup_resource_module(char *path);

extern int32_t cleanup_resource_pool(void);

#endif

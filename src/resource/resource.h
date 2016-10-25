

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

#include "utils/deprecate.h"
#include "memory/memory.h"
#include "io/io.h"
#include <stdint.h>

enum rsrc_gen_type { CACHE, NO_CACHE };

struct resource_ctx
{
    struct memory_block *m_blk;

    void *ptr;
};

struct desc_generator
{
    int32_t (*get_desc)(void);
    int32_t (*free_desc)(int32_t *);
};

struct socket_generator
{
    int32_t (*get_socket)(void);
    int32_t (*free_socket)(int32_t *);
};

struct dirpath_generator
{
    char *(*get_dirpath)(void);
    int32_t (*free_dirpath)(char **);
};

struct filepath_generator
{
    char *(*get_filepath)(void);
    int32_t (*free_filepath)(char **);
};

struct resource_generator
{
    struct desc_generator;
    struct socket_generator;
    struct dirpath_generator;
    struct filepath_generator;
};

extern struct desc_generator *get_default_desc_generator(struct memory_allocator *,
                                                         struct output_writter *);
DEPRECATED extern int32_t get_socket(void);

DEPRECATED extern int32_t free_socket(int32_t *sock_fd);

DEPRECATED extern int32_t get_desc(void);

DEPRECATED extern int32_t free_desc(int32_t *fd);

DEPRECATED extern char *get_mountpath(void);

DEPRECATED extern int32_t free_mountpath(char **path);

DEPRECATED extern char *get_dirpath(void);

DEPRECATED extern int32_t free_dirpath(char **path);

DEPRECATED extern char *get_filepath(void);

DEPRECATED extern int32_t free_filepath(char **path);

DEPRECATED extern int32_t setup_resource_module(enum rsrc_gen_type type, char *path);

DEPRECATED extern int32_t cleanup_resource_module(void);

#endif

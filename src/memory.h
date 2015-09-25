

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

#ifndef MEMORY_H
#define MEMORY_H

#include "private.h"
#include "concurrent.h"

#include <ck_queue.h>
#include <stdlib.h>

static inline void cleanup_buf(char **buf)
{
	if(*buf == NULL)
		return;

	free(*buf);
}

struct memory_block
{
    struct list_node *ptr;

    CK_SLIST_ENTRY(memory_block) list_entry;
};

struct mem_pool_shared
{
    /* Shared memory list the represents the memory pool. */
    CK_SLIST_HEAD(free_list, memory_block) free_list;

    /* Shared memory list the represents the memory pool. */
    CK_SLIST_HEAD(allocated_list, memory_block) allocated_list;
};

private extern void *mem_alloc(unsigned long nbytes);

private extern void *mem_calloc(unsigned long count, unsigned long nbytes);

private extern void mem_free(void *ptr);

private extern void *mem_alloc_shared(unsigned long nbytes);

private extern void *mem_calloc_shared(unsigned long count, unsigned long nbytes);

private extern void mem_free_shared(void *ptr);

private extern struct mem_pool_shared *mem_create_shared_pool(void);

private extern struct memory_block *mem_get_shared_block(struct mem_pool_shared *pool);

private extern void mem_free_shared_block(struct memory_block *block, struct mem_pool_shared *pool);

/* Use this attribute for variables that we want to automatically cleanup. */
#define auto_clean __attribute__((cleanup (cleanup_buf)))

#endif
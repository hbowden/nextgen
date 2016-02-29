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

#ifndef MEMORY_H
#define MEMORY_H

#include "concurrent/concurrent.h"
#include <stdint.h>

struct memory_block
{
    void *ptr;

    NX_SLIST_ENTRY(memory_block);
};

/* Memory pool data structure. */
struct mem_pool_shared
{
    /* Spin lock used to protect underlying data structures in the pool. */
    ck_spinlock_t lock;

    /* The size of the memory block that ptr points at. */
    uint32_t block_size;

    /* The number of blocks in the memory pool. */
    uint32_t block_count;

    /* Shared memory list that represents the memory pool. */
    NX_SLIST_HEAD(free_list, memory_block);

    /* Shared memory list that represents the memory pool. */
    NX_SLIST_HEAD(allocated_list, memory_block);
};

/* Macro for initializing shared memory pool. */
#define init_shared_pool(pool,block) CK_SLIST_FOREACH(block, pool->free_list, list_entry)

/* General purpose heap memory allocator. */
extern void *mem_alloc(uint64_t nbytes);

extern void *mem_calloc(uint64_t nbytes);

extern void mem_free(void **ptr);

extern void *mem_alloc_shared(uint64_t nbytes);

extern void *mem_calloc_shared(uint64_t nbytes);

extern void mem_free_shared(void *ptr, uint64_t nbytes);

extern struct mem_pool_shared *mem_create_shared_pool(uint32_t block_size, uint32_t block_count);

extern void mem_clean_shared_pool(struct mem_pool_shared *pool);

extern struct memory_block *mem_get_shared_block(struct mem_pool_shared *pool);

extern void mem_free_shared_block(struct memory_block *block, struct mem_pool_shared *pool);

/* The auto free function called by auto_free. */
static inline void cleanup_buf(char **buf)
{
    mem_free((void **)buf);
}

/* Use this attribute for variables that we want to automatically free
when they go out of scope. */
#define auto_free __attribute__((cleanup (cleanup_buf)))

#endif

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

#ifndef NX_MEMORY_H
#define NX_MEMORY_H

#include <stdint.h>
#include "concurrent/concurrent.h"

struct memory_block
{
    void *ptr;

    NX_SLIST_ENTRY(memory_block);
};

struct shared_pool
{
    uint64_t block_size;
    uint64_t block_count;

    /* Spin lock used to protect underlying data structures in the pool. */
    ck_spinlock_t lock;

    /* Shared memory list that represents the memory pool. */
    NX_SLIST_HEAD(free_list, memory_block);

    /* Shared memory list that represents the memory pool. */
    NX_SLIST_HEAD(allocated_list, memory_block);
};

struct memory_allocator
{
    void *(*alloc) (uint64_t);
    void *(*shared) (uint64_t);
    void (*free) (void **);
    void (*free_shared) (void **, uint64_t);
    struct shared_pool *(*shared_pool) (uint64_t, uint64_t);
    struct memory_block *(*get_block) (struct shared_pool *);
    void (*free_block) (struct memory_block *, struct shared_pool *pool);
};

/**
 * Macro for initializing a shared memory pool.
 * @param A shared_pool pointer returned from allocator->shared_pool().
 * @param A memory block pointer.
 */
#define init_shared_pool(pool, block) CK_SLIST_FOREACH(block, &pool->free_list, list_entry)

/**
 * This function returns the default memory allocator interface.
 * Use for dynamic and shared memory allocations.
 * @return the default memory allocator.
 */
extern struct memory_allocator *get_default_allocator(void);

#endif

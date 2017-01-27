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

#include "memory.h"
#include "io/io.h"
#include "runtime/platform.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

static void *default_mem_alloc(uint64_t nbytes)
{
    void *ptr = NULL;

    if(nbytes == 0)
        return (NULL);

    ptr = malloc(nbytes);
    if(ptr == NULL)
        return (NULL);

    return (ptr);
}

static void *default_mem_alloc_shared(uint64_t nbytes)
{
    void *pointer = NULL;

    pointer = mmap(NULL, nbytes, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    if(pointer == MAP_FAILED)
        return (NULL);

    return (pointer);
}

void mem_clean_shared_pool(struct mem_pool_shared *pool)
{
    if(pool == NULL)
        return;

    return;
}

struct mem_pool_shared *mem_create_shared_pool(uint32_t block_size,
                                               uint32_t block_count)
{
    /* If the block_size is zero return NULL. */
    if(block_size == 0)
    {
        printf("block_size is zero\n");
        return NULL;
    }

    /* If the block_count is zero return NULL. */
    if(block_count == 0)
    {
        printf("block_count is zero\n");
        return NULL;
    }

    struct mem_pool_shared *pool = NULL;

    /* Allocate the pool structure as shared memory. */
    pool = default_mem_alloc_shared(sizeof(struct mem_pool_shared));
    if(pool == NULL)
    {
        printf("Can't allocate shared memory\n");
        return (NULL);
    }

    /* Initialize the memory pool. */
    struct mem_pool_shared s_pool = {
        .lock = NX_SPINLOCK_INITIALIZER,
        .block_size = block_size,
        .block_count = block_count,
        .free_list = NX_SLIST_HEAD_INITIALIZER(s_pool->free_list),
        .allocated_list = NX_SLIST_HEAD_INITIALIZER(s_pool->allocated_list)
    };

    memmove(pool, &s_pool, sizeof(struct mem_pool_shared));

    /* Init the free and allocated block list. */
    NX_SLIST_INIT(&pool->free_list);
    NX_SLIST_INIT(&pool->allocated_list);

    uint32_t i;

    /* Create blocks of the number requested by the caller of this function. */
    for(i = 0; i < block_count; i++)
    {
        /* Declare memory block. */
        struct memory_block *block = NULL;

        /* Allocate the memory block as shared memory. */
        block = default_mem_alloc_shared(sizeof(struct memory_block));
        if(block == NULL)
        {
            printf("Can't alloc mem_block\n");
            return (NULL);
        }

        /* Allocate enough space for the user to store what they want. */
        block->ptr = default_mem_alloc_shared(block_size);
        if(block->ptr == NULL)
        {
            printf("Can't alloc memory block pointer.\n");
            return (NULL);
        }

        /* Insert the node in the free list. */
        NX_SLIST_INSERT_HEAD(&pool->free_list, block);
    }

    /* Return memory pool pointer. */
    return (pool);
}

struct memory_block *mem_get_shared_block(struct mem_pool_shared *pool)
{
    /* Declare a memory block pointer. */
    struct memory_block *block = NULL;

    /* Loop until we get a shared block. */
    while(1)
    {
        /* Lock the spinlock for exclusive access. */
        nx_spinlock_lock(&pool->lock);

        /* Get first node in list and break out of both loops. */
        NX_SLIST_FOREACH(block, &pool->free_list)
        {
            /* Remove the block from the free_list. */
            NX_SLIST_REMOVE(&pool->free_list, block, memory_block);

            /* Add the block to the allocated block list. */
            NX_SLIST_INSERT_HEAD(&pool->allocated_list, block);

            if(block != NULL)
            {
            /* Were jumping out of the loop, so unlock
               the spin lock now, before jumping. */
                nx_spinlock_unlock(&pool->lock);
                goto done;
            }
        }

        /* Unlock the spinlock. */
        nx_spinlock_unlock(&pool->lock);
    }

done:
    /* Return the empty block. */
    return (block);
}

void mem_free_shared_block(struct memory_block *block,
                           struct mem_pool_shared *pool)
{
    if(block == NULL)
    {
        printf("Block is NULL\n");
        return;
    }

    if(pool == NULL)
    {
        printf("Pool is NULL\n");
        return;
    }

    /* Lock the spinlock. */
    nx_spinlock_lock(&pool->lock);

    /* Remove the block from the allocated_list. */
    NX_SLIST_REMOVE(&pool->allocated_list, block, memory_block);

    /* Add the block to the free block list. */
    NX_SLIST_INSERT_HEAD(&pool->free_list, block);

    /* Unlock the spinlock. */
    nx_spinlock_unlock(&pool->lock);

    return;
}

static void default_mem_free(void **ptr)
{
    /* Return early if the pointer is already NULL. */
    if((*ptr) == NULL)
        return;

    /* Free buffer. */
    free((*ptr));

    /* Set pointer to NULL. */
    (*ptr) = NULL;

    return;
}

static void default_mem_free_shared(void **ptr, uint64_t nbytes)
{
    if((*ptr) == NULL)
        return;

    munmap((*ptr), nbytes);

    (*ptr) = NULL;

    return;
}

struct memory_allocator *get_default_allocator(void)
{
    struct memory_allocator *allocator = NULL;

    allocator = default_mem_alloc(sizeof(struct memory_allocator));
    if(allocator == NULL)
        return (NULL);

    allocator->alloc = &default_mem_alloc;
    allocator->shared = &default_mem_alloc_shared;
    allocator->free = &default_mem_free;
    allocator->free_shared = &default_mem_free_shared;

    return (allocator);
}

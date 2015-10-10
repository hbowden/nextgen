

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

#include "memory.h"
#include "nextgen.h"
#include "io.h"

#include <string.h>
#include <errno.h>
#include <sys/mman.h>

void *mem_alloc(unsigned long nbytes)
{
	void *ptr = NULL;

	if(nbytes > 0)
	{
		ptr = malloc(nbytes);
		if(ptr == NULL)
		{
			output(ERROR, "Can't allocate: %lu bytes, because: %s\n", nbytes, strerror(errno));
			return NULL;
		}
	}
	else
	{
        output(ERROR, "Can't allocate zero bytes\n");
	    return NULL;
	}

	return ptr;
}

void *mem_calloc(unsigned long count, unsigned long nbytes)
{
    void  *ptr = NULL;

    if(count > 0 || nbytes > 0)
    {
    	ptr = calloc(count, nbytes);
    	if(ptr == NULL)
    	{
            output(ERROR, "Can't allocate and initialize: %lu bytes: because: %s\n", nbytes, strerror(errno));
            return NULL;
    	}
    }
    else
    {
    	output(ERROR, "Count or nbytes was not greater than zero\n");
    	return NULL;
    }

    return ptr;
}

void mem_free(void *ptr)
{
    if(ptr == NULL)
    	return;

    free(ptr);
}

void *mem_alloc_shared(unsigned long nbytes)
{
	void *pointer = NULL;

	pointer = mmap(NULL, nbytes, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    if(pointer == MAP_FAILED)
    {
        output(ERROR, "mmap: %s\n", strerror(errno));
        return NULL;
    }

    return pointer;
}

void *mem_calloc_shared(unsigned long count, unsigned long nbytes)
{
    return NULL;
}

void mem_free_shared(void *ptr, unsigned long nbytes)
{
    if(ptr == NULL)
        return;

    munmap(ptr, nbytes);

    return;
}

void clean_shared_pool(struct mem_pool_shared *pool)
{

    return;
}

struct mem_pool_shared *mem_create_shared_pool(unsigned int block_size, unsigned int block_count)
{
    /* If the block_size is zero return NULL. */
    if(block_size == 0)
    {
        output(ERROR, "block_size is zero\n");
        return NULL;
    }

    /* Intialize the memory pool. */
	struct mem_pool_shared s_pool = {

        .lock = CK_SPINLOCK_INITIALIZER,
        .block_size = block_size,
        .block_count = block_count,
        .free_list = CK_SLIST_HEAD_INITIALIZER(s_pool->free_list),
        .allocated_list = CK_SLIST_HEAD_INITIALIZER(s_pool->allocated_list)

    };

    /* Declare a pointer to the struct. */
    struct mem_pool_shared *pool = &s_pool;

    /* Allocate the pool structure as shared memory. */
	pool = mem_alloc_shared(sizeof(struct mem_pool_shared));
	if(pool == NULL)
	{
		output(ERROR, "Can't allocate shared memory\n");
		return NULL;
	}

    /* Init the free and allocated block list. */
	CK_SLIST_INIT(&pool->free_list);
	CK_SLIST_INIT(&pool->allocated_list);

	unsigned int i;

    /* Create blocks of the number requested by the user. */
	for(i = 0; i < block_count; i++)
	{
        /* Declare memory block. */
        struct memory_block *block = NULL;

        /* Allocate the memory block as shared memory. */
        block = mem_alloc_shared(sizeof(struct memory_block));
        if(block == NULL)
        {
        	output(ERROR, "Can't alloc mem_block\n");
        	return NULL;
        }

        /* Allocate enough space for the user to store what they want. */
        block->ptr = mem_alloc_shared(block_size);
        if(block->ptr == NULL)
        {
        	output(ERROR, "Can't alloc memory block pointer.\n");
        	return NULL;
        }

		/* Insert the node in the free list. */
        CK_SLIST_INSERT_HEAD(&pool->free_list, block, list_entry);
	}

    /* Return memory pool pointer. */
	return pool;
}

struct memory_block *mem_get_shared_block(struct mem_pool_shared *pool)
{
    /* Check if the arg cleanup list is empty, if it is empty return early. */
    if(CK_SLIST_EMPTY(&pool->free_list) == TRUE)
        return NULL;

    /* Lock the spinlock for mutual access. */
    ck_spinlock_lock(&pool->lock);

    /* Declare a memory block pointer. */
    struct memory_block *block = NULL;
   
    /* Loop for each node in the list. */
    CK_SLIST_FOREACH(block, &pool->free_list, list_entry)
    {
    	/* Remove the block from the free_list. */
    	CK_SLIST_REMOVE(&pool->free_list, block, memory_block, list_entry);

    	/* Add the block to the allocated block list. */
    	CK_SLIST_INSERT_HEAD(&pool->allocated_list, block, list_entry);

        /* Unlock the spinlock. */
        ck_spinlock_unlock(&pool->lock);

    	/* Return the empty block. */
        return block;
    }

    /* Unlock the spinlock. */
    ck_spinlock_unlock(&pool->lock);

    /* Should not get here. */
    return NULL;
}


void mem_free_shared_block(struct memory_block *block, struct mem_pool_shared *pool)
{
    /* Lock the spinlock for mutual access. */
    ck_spinlock_lock(&pool->lock);

	/* Remove the block from the allocated_list. */
    CK_SLIST_REMOVE(&pool->allocated_list, block, memory_block, list_entry);

    /* Add the block to the free block list. */
    CK_SLIST_INSERT_HEAD(&pool->free_list, block, list_entry);

    /* Unlock the spinlock. */
    ck_spinlock_unlock(&pool->lock);

    return;
}

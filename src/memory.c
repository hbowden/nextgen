

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

void mem_free_shared(void *ptr)
{
    return;
}

struct mem_pool_shared *mem_create_shared_pool(void)
{
	struct mem_pool_shared s_pool = {
        
        .free_list = CK_SLIST_HEAD_INITIALIZER(child->pool->free_list),
        .allocated_list = CK_SLIST_HEAD_INITIALIZER(child->pool->allocated_list)
    };

    struct mem_pool_shared *pool = &s_pool;

    /* Allocate the pool structure as shared memory. */
	pool = mem_alloc_shared(sizeof(struct mem_pool_shared));
	if(pool == NULL)
	{
		output(ERROR, "Can't allocate shared memory\n");
		return NULL;
	}

	CK_SLIST_INIT(&pool->free_list);
	CK_SLIST_INIT(&pool->allocated_list);

	unsigned int i;

    /* Create 256 Memory blocks. */
	for(i = 0; i < 2048; i++)
	{
        struct memory_block *block = NULL;

        block = mem_alloc_shared(sizeof(struct memory_block));
        if(block == NULL)
        {
        	output(ERROR, "Can't alloc mem_block\n");
        	return NULL;
        }

        struct list_node *node = NULL;

        node = mem_alloc_shared(sizeof(struct list_node));
        if(node == NULL)
        {
        	output(ERROR, "Can't alloc list node\n");
        	return NULL;
        }

        struct list_data *data = NULL;

        data = mem_alloc_shared(sizeof(struct list_data));
        if(data == NULL)
        {
        	output(ERROR, "Can't alloc list data\n");
        	return NULL;
        }

        block->ptr = node;
        block->ptr->data = data;

        block->ptr->data->path = mem_alloc_shared(1025);
        if(block->ptr->data->path == NULL)
        {
            output(ERROR, "Can't alloc path\n");
            return NULL;
        }

		/* Insert the node in the free list. */
        CK_SLIST_INSERT_HEAD(&pool->free_list, block, list_entry);
	}

	return pool;
}

struct memory_block *mem_get_shared_block(struct mem_pool_shared *pool)
{
    /* Check if the arg cleanup list is empty, if it is empty return early. */
    if(CK_SLIST_EMPTY(&pool->free_list) == TRUE)
        return NULL;

    struct memory_block *block = NULL;
   
    /* Loop for each node in the list. */
    CK_SLIST_FOREACH(block, &pool->free_list, list_entry)
    {
    	/* Remove the block from the free_list. */
    	CK_SLIST_REMOVE(&pool->free_list, block, memory_block, list_entry);

    	/* Add the block to the allocated block list. */
    	CK_SLIST_INSERT_HEAD(&pool->allocated_list, block, list_entry);

    	/* Return the first empty block. */
        return block;
    }

    /* Should not get here. */
    return NULL;
}


void mem_free_shared_block(struct memory_block *block, struct mem_pool_shared *pool)
{
	/* Remove the block from the allocated_list. */
    CK_SLIST_REMOVE(&pool->allocated_list, block, memory_block, list_entry);

    /* Add the block to the free block list. */
    CK_SLIST_INSERT_HEAD(&pool->free_list, block, list_entry);

    return;
}


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

#include "concurrent.h"
#include "platform.h"
#include "nextgen.h"
#include "memory.h"
#include "types.h"
#include "io.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

/* CAS loop for swapping atomic int32 values. */ 
int32_t cas_loop_int32(atomic_int_fast32_t *target, int32_t value)
{
	/* Loop until we can succesfully update the the value. */
    while(1)
    {
        /* Grab a snapshot of the value that need to be updated. */
        int32_t snapshot = atomic_load(target);

        /* Try swaping the variable. */
        if(atomic_compare_exchange_weak(target, &snapshot, value) == TRUE)
        {
            /* We succesfully updated the value let's exit this loop and return. */
            break;
        }
    }

	return 0;
}

/* CAS loop for swapping atomic uint32 values. */ 
int32_t cas_loop_uint32(atomic_uint_fast32_t *target, uint32_t value)
{
	/* Loop until we can succesfully update the the value. */
    while(1)
    {
        /* Grab a snapshot of the value that need to be updated. */
        uint32_t snapshot = atomic_load(target);

        /* Try swaping the variable. */
        if(atomic_compare_exchange_weak(target, &snapshot, value) == TRUE)
        {
            /* We succesfully updated the value let's exit this loop and return. */
            break;
        }
    }

	return 0;
}

int32_t wait_on(atomic_int_fast32_t *pid, int32_t *status)
{
    waitpid(atomic_load(pid), status, 0);

    return 0;
}

int32_t create_work_queue(struct work_queue *queue, uint32_t block_size)
{
    /* Initialize the work queue. */
    struct work_queue w_queue = {

        .lock = SPINLOCK_INITIALIZER,
        .list = SLIST_HEAD_INITIALIZER(w_queue.list),
        .block_size = block_size

    };

    queue = &w_queue;

    /* Allocate the queue as shared memory. */
    queue = mem_alloc_shared(sizeof(struct work_queue));
    if(queue == NULL)
    {
        output(ERROR, "Can't alloc work queue\n");
        return -1;
    }

    /* Initialize the linked list. */
    CK_SLIST_INIT(&queue->list);

    /* Create the shared memory pool that we will grab queue blocks from. */
    queue->pool = mem_create_shared_pool(sizeof(struct queue_block), 16384);
    if(queue->pool == NULL)
    {
        output(ERROR, "Can't create shared pool\n");
        return -1;
    }

    struct memory_block *m_blk = NULL;

    /* Loop for each memory block in pool. */
    init_shared_pool(&queue->pool, m_blk)
    {
        struct queue_block *q_blk = NULL;

        /* Allocate a queue block as shared memory. */
        q_blk = mem_alloc_shared(sizeof(struct queue_block));
        if(q_blk == NULL)
        {
            output(ERROR, "Can't allocate a queue block\n");
            return -1;
        }

        /* Allocate the storage pointer as shared anonymous memory. */
        q_blk->ptr = mem_alloc_shared(block_size);
        if(q_blk->ptr == NULL)
        {
            output(ERROR, "Can't allocate block pointer\n");
            return -1;
        }

        /* Insert the queue block into the shared pool. */
        m_blk->ptr = q_blk;
    }

    return 0;

}

int destroy_work_queue(struct work_queue *queue)
{

    return 0;
}

struct queue_block *get_queue_block(struct work_queue *queue)
{
    struct memory_block *m_blk = NULL;

    /* Grab a memory block from the shared pool. */
    m_blk = mem_get_shared_block(queue->pool);
    if(m_blk == NULL)
    {
        output(ERROR, "Can't get memory block\n");
        return NULL;
    }

    /* Set the queue block to the one grabbed from the pool. */
    struct queue_block *q_blk = (struct queue_block *) m_blk->ptr;

    /* Set the memory block pointer. */
    q_blk->m_blk = m_blk;

    return (q_blk);
}

int32_t insert_into_queue(struct queue_block *q_blk, struct work_queue *queue)
{
    /* Lock the spinlock for mutual access. */
    ck_spinlock_lock(&queue->lock);

    /* Insert the queue block into the pool. */
    CK_SLIST_INSERT_HEAD(&queue->list, q_blk, list_entry);

    /* Unlock the spinlock. */
    ck_spinlock_unlock(&queue->lock);

    return (0);
}

void *get_from_queue(struct work_queue *queue)
{
    struct queue_block *q_blk = NULL;

    /* Loop for each node in the list. */
    CK_SLIST_FOREACH(q_blk, &queue->list, list_entry)
    {
        /* Lock the spinlock for mutual access. */
        ck_spinlock_lock(&queue->lock);

        /* Remove the q_blk from the queue.  */
        CK_SLIST_REMOVE(&queue->list, q_blk, queue_block, list_entry);

        /* Unlock the spinlock. */
        ck_spinlock_unlock(&queue->lock);

        /* Free the shared memory block. */
        mem_free_shared_block((struct memory_block *)(q_blk)->ptr, queue->pool);

        /* Return the pointer that the q_blk holds. */
        return ((q_blk)->ptr);
    }

    /* We should not get here. */
    return NULL;
}


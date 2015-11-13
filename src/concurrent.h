

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

#ifndef CONCURRENT_H
#define CONCURRENT_H

#include "ck_queue.h"
#include "ck_spinlock.h"
#include "stdatomic.h"

#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>

#define LIST_ENTRY(x) CK_LIST_ENTRY(x) list_entry
#define SLIST_ENTRY(x) CK_SLIST_ENTRY(x) list_entry

#define LIST_HEAD(name,type) 
#define SLIST_HEAD(name,type) CK_SLIST_HEAD(name, type) name

#define SPINLOCK_INITIALIZER CK_SPINLOCK_INITIALIZER
#define SLIST_HEAD_INITIALIZER CK_SLIST_HEAD_INITIALIZER

typedef ck_spinlock_t spinlock_t;

struct queue_block
{
    void *ptr;

    void *m_blk;

    SLIST_ENTRY(queue_block);
};

struct work_queue
{
    spinlock_t lock;

    uint32_t block_size;

    struct mem_pool_shared *pool;

    SLIST_HEAD(list, queue_block);
};

/* CAS loop for swapping atomic int32 values. */ 
extern int32_t cas_loop_int32(atomic_int_fast32_t *target, int32_t value);

/* CAS loop for swapping atomic uint32 values. */ 
extern int32_t cas_loop_uint32(atomic_uint_fast32_t *target, uint32_t value);

/* Simple wrapper function so we can wait on atomic pid values.  */
extern int32_t wait_on(atomic_int_fast32_t *pid, int32_t *status);

/* Creates a shared memory work queue. */
extern int32_t create_work_queue(struct work_queue *queue, uint32_t size);

/* Destroy/cleanup a existing work queue. */
extern int32_t destroy_work_queue(struct work_queue *queue);

extern struct queue_block *get_queue_block(struct work_queue *queue);

/* Insert a pointer into the queue. */
extern int32_t insert_into_queue(struct queue_block *q_blk, struct work_queue *queue);

/* Get a pointer from the queue. */
extern void *get_from_queue(struct work_queue *queue);

#endif

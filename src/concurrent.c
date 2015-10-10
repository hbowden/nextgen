

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
#include "nextgen.h"
#include "memory.h"
#include "io.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>


/* CAS loop for swapping atomic int32 values. */ 
int cas_loop_int32(atomic_int_fast32_t *target, int value)
{
	/* Loop until we can succesfully update the the value. */
    while(1)
    {
        /* Grab a snapshot of the value that need to be updated. */
        int snapshot = atomic_load(target);

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
int cas_loop_uint32(atomic_uint_fast32_t *target, unsigned int value)
{
	/* Loop until we can succesfully update the the value. */
    while(1)
    {
        /* Grab a snapshot of the value that need to be updated. */
        unsigned int snapshot = atomic_load(target);

        /* Try swaping the variable. */
        if(atomic_compare_exchange_weak(target, &snapshot, value) == TRUE)
        {
            /* We succesfully updated the value let's exit this loop and return. */
            break;
        }
    }

	return 0;
}

int wait_on(atomic_int_fast32_t *pid, int *status)
{

    waitpid(atomic_load(pid), status, 0);

    return 0;
}

int create_work_queue(struct work_queue *queue)
{
    struct work_queue w_queue = {

        .lock = CK_SPINLOCK_INITIALIZER,
        .queue = CK_SLIST_HEAD_INITIALIZER(w_queue.organism_list)

    };

    queue = mem_alloc_shared(sizeof(struct work_queue));
    if(queue == NULL)
    {
        output(ERROR, "Can't alloc work queue\n");
        return -1;
    }



    queue = &w_queue;

    return 0;

}


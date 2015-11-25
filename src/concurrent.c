

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
#include "io.h"

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

/* CAS loop for swapping atomic int32 values. */ 
void cas_loop_int32(atomic_int_fast32_t *target, int32_t value)
{
	/* Loop until we can succesfully update the the value. */
    while(1)
    {
        /* Grab a snapshot of the value that need to be updated. */
        int32_t snapshot = atomic_load(target);

        /* Try swaping the variable if successful break from the loop. */
        if(atomic_compare_exchange_weak(target, &snapshot, value) == TRUE)
            break;
    }
}

/* CAS loop for swapping atomic uint32 values. */ 
void cas_loop_uint32(atomic_uint_fast32_t *target, uint32_t value)
{
	/* Loop until we can succesfully update the the value. */
    while(1)
    {
        /* Grab a snapshot of the value that need to be updated. */
        uint32_t snapshot = atomic_load(target);

        /* Try swaping the variable if successful break from the loop. */
        if(atomic_compare_exchange_weak(target, &snapshot, value) == TRUE)
            break;
    }
}

int32_t wait_on(atomic_int_fast32_t *pid, int32_t *status)
{
    waitpid(atomic_load(pid), status, 0);

    return 0;
}

#ifdef MAC_OSX

struct message
{
    mach_msg_header_t header;
    void *data;
};

int32_t msg_send(msg_port_t send_port, msg_port_t remote_port, void *msg)
{
    /* Make sure the message buffer is not NULL. */
    if(msg == NULL)
    {
        output(ERROR, "Message buffer is NULL\n");
        return (-1);
    }

    //mach_msg();


    return (0);
}

int32_t msg_recv(msg_port_t recv_port, void *buffer)
{
    return (0);
}

msg_port_t init_msg_port(void)
{
    int32_t rtrn = 0;
    msg_port_t port = 0;

    rtrn = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port);
    if(rtrn < 0)
    {
        output(ERROR, "Can't allocate mach port\n");
        return (-1);
    }
    
    return (port);
}

#endif

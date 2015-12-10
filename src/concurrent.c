

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

#include "concurrent.h"
#include "platform.h"
#include "nextgen.h"
#include "memory.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifdef MAC_OSX

#include <mach/error.h>
#include <mach/message.h>

#endif

#ifdef FREEBSD

#include <mqueue.h>

#endif

/* Compare and swap loop for swapping atomic int32 values. */ 
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

    return (0);
}

#ifdef FREEBSD

msg_port_t init_msg_port(void)
{
    msg_port_t port = 0;

    return (port);
}

int32_t msg_send(msg_port_t send_port, msg_port_t remote_port, void *msg_data)
{


    return (0);
}

void *msg_recv(msg_port_t recv_port)
{
    void *message = NULL;

    return (message);
}

#endif

#ifdef MAC_OSX

#define DEFAULT_MSG_ID 400

typedef struct
{
    mach_msg_header_t header;

} msg_format_request_t;

typedef struct
{
    mach_msg_header_t header;
    mach_msg_trailer_t trailer;

} msg_format_request_r_t;

typedef struct
{
    mach_msg_header_t header;
    mach_msg_body_t body;
    mach_msg_ool_descriptor_t data;
    mach_msg_type_number_t count;

} msg_format_response_t;

typedef struct
{
    mach_msg_header_t header;
    mach_msg_body_t body;
    mach_msg_ool_descriptor_t data;
    mach_msg_type_number_t count;
    mach_msg_trailer_t trailer;

} msg_format_response_r_t;

int32_t fork_pass_port(msg_port_t pass_port, int32_t (*proc_start)(void *arg), void *arg)
{
    pid_t pid = 0;
    int32_t rtrn = 0;
    kern_return_t kr = 0;
    mach_port_t special_port = 0;

    /* Grab our current task's(process's) HOST_NAME special port. */
    kr = task_get_special_port(mach_task_self(), TASK_HOST_PORT, &special_port);
    if(kr < 0)
    {
        mach_error("Can't get special port:\n", kr);
        return (-1);
    }

    /* Set the HOST_NAME special port with the msg port we wan't to pass to the child procees. */
    kr = task_set_special_port(mach_task_self(), TASK_HOST_PORT, (mach_port_t)pass_port);
    if(kr < 0)
    {
        mach_error("Can't get special port:\n", kr);
        return (-1);
    }

    pid = fork();
    if(pid == 0)
    {
        mach_port_t parent_port = 0;

        /* Grab our current task's(process's) HOST_NAME special port which should be the port passed
        from our parrent process. */
        kr = task_get_special_port(mach_task_self(), TASK_HOST_PORT, &parent_port);
        if(kr < 0)
        {
            mach_error("Can't get special port:\n", kr);
            _exit(-1);
        }

        /* Now that we have the parent's message port let's recieve a message from the parent so we can
        restore our special port. */

        struct 

        msg_recv()


        /* Now that we successfully passed our mach port let's call the function pointer passed by the caller. */
        rtrn = proc_start(arg);
        if(rtrn < 0)
        {
            output(ERROR, "proccess start function failed\n");
            _exit(-1);
        }
    }
    else if(pid > 0)
    {
        /* Reset our special port to it's orginal value. */
        kr = task_set_special_port(mach_task_self(), TASK_HOST_PORT, special_port);
        if(kr < 0)
        {
            mach_error("Can't get special port:\n", kr);
            return (-1);
        }


    }
    else
    {
        output(ERROR, "Can't create child process: %s\n", strerror(errno));
        return (-1);
    }

    return (int32_t)(pid);
}

int32_t msg_send(msg_port_t send_port, msg_port_t remote_port, void *msg_data)
{
    /* Make sure the message buffer is not NULL. */
    if(msg_data == NULL)
    {
        output(ERROR, "Message data is NULL\n");
        return (-1);
    }

    kern_return_t kr = 0;
    msg_format_response_t send_msg;
    mach_msg_header_t *send_hdr = NULL;

    /* Setup message header. */
    send_hdr = &(send_msg.header);
    send_hdr->msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND);
    send_hdr->msgh_bits |= MACH_MSGH_BITS_COMPLEX;
    send_hdr->msgh_size = sizeof(send_msg);
    send_hdr->msgh_local_port = send_port;
    send_hdr->msgh_remote_port = remote_port;
    send_hdr->msgh_id = DEFAULT_MSG_ID;
    
    /* Setup message body. */
    send_msg.body.msgh_descriptor_count = 0;
    send_msg.data.address = (void *)msg_data;
    send_msg.data.size = sizeof(msg_data) + 1;
    send_msg.data.deallocate = FALSE;
    send_msg.data.copy = MACH_MSG_VIRTUAL_COPY;
    send_msg.data.type = MACH_MSG_OOL_DESCRIPTOR;
    send_msg.count = send_msg.data.size;

    /* Send mach message. */
    kr = mach_msg(send_hdr,
                  MACH_SEND_MSG,
                  send_hdr->msgh_size,
                  0,
                  MACH_PORT_NULL,
                  MACH_MSG_TIMEOUT_NONE,
                  MACH_PORT_NULL);
    if(kr != MACH_MSG_SUCCESS)
    {
        mach_error("Can't send mach message:\n", kr);
        return (-1);
    }

    return (0);
}

void *msg_recv(msg_port_t recv_port)
{
    kern_return_t kr = 0;
    msg_format_response_t recv_msg = NULL;
    mach_msg_header_t *recv_hdr = NULL;

    recv_hdr = &recv_msg.header;
    recv_hdr->msgh_local_port = recv_port;
    recv_hdr->msgh_size = sizeof(recv_msg);

    kr = mach_msg(recv_hdr,
                  MACH_RCV_MSG,
                  0,
                  recv_hdr->msgh_size,
                  recv_port,
                  MACH_MSG_TIMEOUT_NONE,
                  MACH_PORT_NUL);

    if(kr != MACH_MSG_SUCCESS)
    {
        mach_error("Can't recieve mach message:\n", kr);
        return (-1);
    }

    return (recv_msg.data);
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

/**
 * Copyright (c) 2016, Harrison Bowden, Minneapolis, MN
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

#include "concurrent-mac.h"
#include "io/io.h"

#include <stdlib.h>

#define SPECIAL_PORT TASK_BOOTSTRAP_PORT

int32_t init_msg_port(msg_port_t *msg_port)                      
{
    kern_return_t err;
    mach_port_t port = MACH_PORT_NULL;

    err = mach_port_allocate(mach_task_self (),
                              MACH_PORT_RIGHT_RECEIVE, &port);
    if(err != KERN_SUCCESS)
    {
        mach_error("Can't allocate mach port\n", err);
        return (-1);
    }

    err = mach_port_insert_right(mach_task_self (),
                                  port,
                                  port,
                                  MACH_MSG_TYPE_MAKE_SEND);
    if(err != KERN_SUCCESS)
    {
        mach_error("Can't insert port right\n", err);
        return (-1);
    }

    (*msg_port) = port;
    
    return (0);
}

static int32_t
send_port(mach_port_t remote_port, mach_port_t port)
{
    kern_return_t err;

    struct
    {
        mach_msg_header_t          header;
        mach_msg_body_t            body;
        mach_msg_port_descriptor_t task_port;
    } msg;

    msg.header.msgh_remote_port = remote_port;
    msg.header.msgh_local_port = MACH_PORT_NULL;
    msg.header.msgh_bits = MACH_MSGH_BITS (MACH_MSG_TYPE_COPY_SEND, 0) |
        MACH_MSGH_BITS_COMPLEX;
    msg.header.msgh_size = sizeof msg;

    msg.body.msgh_descriptor_count = 1;
    msg.task_port.name = port;
    msg.task_port.disposition = MACH_MSG_TYPE_COPY_SEND;
    msg.task_port.type = MACH_MSG_PORT_DESCRIPTOR;

    err = mach_msg_send(&msg.header);
    if(err != KERN_SUCCESS)
    {
        mach_error("Can't send mach msg\n", err);
        return (-1);
    }

    return (0);
}

static int32_t
recv_port(mach_port_t recv_port, mach_port_t *port)
{
    kern_return_t err;
    struct
    {
        mach_msg_header_t          header;
        mach_msg_body_t            body;
        mach_msg_port_descriptor_t task_port;
        mach_msg_trailer_t         trailer;
    } msg;

    err = mach_msg(&msg.header, MACH_RCV_MSG,
                    0, sizeof msg, recv_port,
                    MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    if(err != KERN_SUCCESS)
    {
        mach_error("Can't recieve mach message\n", err);
        return (-1);
    }

    (*port) = msg.task_port.name;
    return 0;
}

pid_t
fork_pass_port(mach_port_t *pass_port,
               int32_t (*child_start)(mach_port_t port, void *arg),
               void *arg)
{
    pid_t pid = 0;
    int32_t rtrn = 0;
    kern_return_t err;
    mach_port_t special_port = MACH_PORT_NULL;

    /* Allocate the message port. */
    if(init_msg_port(pass_port) != 0)
    {
        output(ERROR, "Can't setup mach port\n");
        return (-1);
    }

    /* Grab our current process's bootstrap port. */
    err = task_get_special_port(mach_task_self(), SPECIAL_PORT, &special_port);
    if(err != KERN_SUCCESS)
    {
        mach_error("Can't get special port:\n", err);
        return (-1);
    }

    /* Set the special port as the parent recv port.  */
    err = task_set_special_port(mach_task_self(), SPECIAL_PORT, (*pass_port));
    if(err != KERN_SUCCESS)
    {
        mach_error("Can't set special port:\n", err);
        return (-1);
    }

    pid = fork();
    if(pid == 0)
    {
        mach_port_t original_bootstrap_port = MACH_PORT_NULL;
        mach_port_t port = MACH_PORT_NULL;

        /* In the child process grab the port passed by the parent. */
        err = task_get_special_port(mach_task_self(), SPECIAL_PORT, pass_port);
        if(err != KERN_SUCCESS)
        {
            mach_error("Can't get special port:\n", err);
            return (-1);
        }

        /* Create a port with a send right. */
        if(init_msg_port(&port) != 0)
        {
            output(ERROR, "Can't setup mach port\n");
            return (-1);
        }
        
        /* Send port to parent. */
        rtrn = send_port((*pass_port), port);
        if(rtrn < 0)
        {
            output(ERROR, "Can't send port\n");
            return (-1);
        }
        
        /* Receive the real bootstrap port from the parent. */
        rtrn = recv_port(port, &original_bootstrap_port);
        if(rtrn < 0)
        {
            output(ERROR, "Can't receive bootstrap port\n");
            return (-1);
        }
        
        /* Set the bootstrap port back to normal. */
        err = task_set_special_port(mach_task_self(), SPECIAL_PORT, original_bootstrap_port);
        if(err != KERN_SUCCESS)
        {
            mach_error("Can't set special port:\n", err);
            return (-1);
        }

        /* Now that were done with the port dance, start the function passed by the caller. */
        child_start((*pass_port), arg);
        
        /* Exit and clean up the child process. */
        _exit(0);
    }
    else if(pid > 0)
    {
        mach_port_t child_port = MACH_PORT_NULL;

        /* Grab the child's recv port. */
        rtrn = recv_port((*pass_port), &child_port);
        if(rtrn < 0)
        {
            output(ERROR, "Can't recv port\n");
            return (-1);
        }
        
        /* Send the child the original bootstrap port. */
        rtrn = send_port(child_port, special_port);
        if(rtrn < 0)
        {
            output(ERROR, "Can't send bootstrap port\n");
            return (-1);
        }

        /* Reset parents special port. */
        err = task_set_special_port(mach_task_self(), SPECIAL_PORT, special_port);
        if(err != KERN_SUCCESS)
        {
            mach_error("Can't set special port:\n", err);
            return (-1);
        }
        
        return (pid);
    }
    else
    {
        /* Error, so cleanup the mach port. */
        err = mach_port_deallocate(mach_task_self(), (*pass_port));
        if(err != KERN_SUCCESS)
        {
            mach_error("Can't deallocate mach port\n", err);
            return (-1);
        }
    
        perror("fork");

        return (-1);
    }
}

int32_t msg_send(msg_port_t send_port, msg_port_t remote_port, void *msg_data)
{
    if(msg_data == NULL)
    {
        output(ERROR, "Msg buf is NULL\n");
        return (-1);
    }



	return (0);
}

void *msg_recv(msg_port_t recv_port)
{
	void *data = NULL;

	return (data);
}

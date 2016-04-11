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

#include "concurrent.h"
#include "memory/memory.h"
#include "io/io.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

struct msg_port
{
    int32_t port[2];
};

struct message_ctx
{
	void *data;

    size_t size;
};

int32_t init_msg_port(msg_port_t *port)
{
    port = mem_alloc(sizeof(struct msg_port));
    if(port == NULL)
    {
        output(ERROR, "Can't allocate message port\n");
        return (-1);
    }

    /* Create a new pipe. */
	int32_t rtrn = pipe(port->port);
	if(rtrn < 0)
	{
        output(ERROR, "Can't create pipe: %s\n", strerror(errno));
        return (-1);
	}

    return (0);
}

int32_t msg_send(msg_port_t remote_port, void *data, uint32_t size)
{
	ssize_t ret = 0;
    size_t msg_len = sizeof(struct message_ctx);
	struct message_ctx *msg = NULL;

    msg = mem_alloc(msg_len);
    if(msg == NULL)
    {
        output(ERROR, "Can't allocate message\n");
        return (-1);
    }

	msg->size = size;
	msg->data = data;

	ret = write(remote_port.port[1], &msg, msg_len);
	if(ret < (ssize_t)size)
	{
		output(ERROR, "Can't write message: %s\n", strerror(errno));
		return (-1);
	}

    return (0);
}

void *msg_recv(msg_port_t recv_port)
{
	ssize_t ret = 0;
	struct message_ctx *msg = NULL;
	size_t msg_len = sizeof(struct message_ctx);

    msg = mem_alloc(msg_len);
    if(msg == NULL)
    {
        output(ERROR, "Can't allocate message\n");
        return (NULL);
    }

    ret = read(recv_port.port[1], &msg, msg_len);
    if(ret < (ssize_t)msg_len)
    {
    	output(ERROR, "Can't read message: %s\n", strerror(errno));
    	return (NULL);
    }

    return (msg->data);
}

int32_t fork_pass_port(msg_port_t *pass_port, int32_t (*proc_start)(msg_port_t port, void *arg), void *arg)
{
	pid_t pid = 0;
	int32_t rtrn = 0;

	/* Initialize the port passed in. */
	rtrn = init_msg_port(pass_port);
	if(rtrn < 0)
	{
		output(ERROR, "Can't initialze message port\n");
		return (-1);
	}

	pid = fork();
    if(pid == 0)
    {
        rtrn = proc_start((*pass_port), arg);
        if(rtrn < 0)
        {
        	output(ERROR, "Can't start child\n");
        	return (-1);
        }

        _exit(0);
    }
    else if(pid > 0)
    {
        return (pid);
    }
    else
    {
        output(ERROR, "Can't create child proc: %s\n", strerror(errno));
        return (-1);
    }
}

int32_t recv_port(msg_port_t recv_port, msg_port_t *port)
{
    port = msg_recv(recv_port);
    if(port == NULL)
    {
    	output(ERROR, "Can't recv message\n");
    	return (-1);
    }

    return (0);
}

int32_t send_port(msg_port_t remote_port, msg_port_t port)
{
    return (msg_send(remote_port, &port, sizeof(msg_port_t)));
}

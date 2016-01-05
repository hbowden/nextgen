

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

#include "test_utils.h"
#include "../../src/concurrent.h"
#include "../../src/memory.h"

#include <string.h>
#include <errno.h>
#include <sys/wait.h>

static int32_t child_proc_start(msg_port_t remote_port, void *arg)
{
    char *recv_buffer = NULL;

    recv_buffer = mem_alloc(10);
    if(recv_buffer == NULL)
        _exit(-1);

    recv_buffer = (char *) msg_recv(remote_port);
    assert_stat(recv_buffer != NULL);

    /* Make sure we got the message. */
    assert_stat(strncmp(recv_buffer, "123456789", 9) == 0);
        
    /* Exit and clean up the child process. */
     _exit(0);
}

static int test_msg_send_recv(void)
{
	log_test(DECLARE, "Testing msg send and msg recv");

    int32_t rtrn = 0;

    /* Declare a message port to send data on. */
    msg_port_t send_port = 0;
    
    /* Declare the message port to send data to. */
    msg_port_t remote_port = 0;

    /* Initialize the send port. */
    send_port = init_msg_port();

    /* Initialize the remote port */
    remote_port = init_msg_port();

    /* Send port should be greater than zero. */
    assert_stat(send_port > 0);

    /* Declare a buffer to send via msg_send() */
    char *buffer = NULL;

    /* msg_send() should fail with a NULL buffer. */
	rtrn = msg_send(send_port, remote_port, buffer);
	assert_stat(rtrn == -1);

    /* Allocate the buffer. */
	buffer = mem_alloc(10);
    if(buffer == NULL)
    	return (-1);

    /* Give the buffer a value. */
    memmove(buffer, "123456789", 9);

	pid_t pid = 0;

	/* Fork and pass the child the remote port. */
    pid = fork_pass_port(remote_port, child_proc_start, NULL);
    assert_stat(pid > 0);
   
    /* Send a message to the child process. */
    rtrn = msg_send(send_port, remote_port, buffer);
	assert_stat(rtrn == 0);

    int32_t status = 0;

    /* Wait for the child process. */
    wait4(pid, &status, 0, NULL);

    /* Make sure the process exited normally, if not return an error. */
    if(WIFEXITED(status) != 0)
    {
        output(ERROR, "Bad return value\n");

        return (-1);
            
    }
    else if(WIFSIGNALED(status) != 0)
    {
        output(ERROR, "Signal recieved\n");

        return (-1);
    }

    log_test(SUCCESS, "msg send and recv test passed");

	return (0);
}

int main(void)
{
	int32_t rtrn = 0;

    rtrn = init_test_framework();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init test framework");
        return (-1);
    }

    /* Initialize the stats object. */
    test_stat = init_stats_obj();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't init the stats object\n");
        return (-1);
    }

    /* Test sending and recieving IPC messages. */
    rtrn = test_msg_send_recv();
    if(rtrn < 0)
        log_test(FAIL, "Msg send and recv test failed");


	return (0);
}

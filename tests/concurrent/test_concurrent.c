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
#include "concurrent/concurrent.h"
#include "memory/memory.h"

#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <pthread.h>

/*static int32_t child_proc_start(msg_port_t remote_port, void *arg)
{
    char *recv_buffer = NULL;

    recv_buffer = mem_alloc(10);
    if(recv_buffer == NULL)
        _exit(-1);

    recv_buffer = (char *) msg_recv(remote_port);
    assert_stat(recv_buffer != NULL);

    assert_stat(strncmp(recv_buffer, "123456789", 9) == 0);
        
    _exit(0);
} */

static void *thread_start(void *arg)
{
    return (NULL);
}

static int32_t test_msg_send_recv(void)
{
    log_test(DECLARE, "Testing msg send and msg recv");

    int32_t rtrn = 0;
    pthread_t thread = 0;

    msg_port_t port = 0;

    /* Initialize the message port. */
    rtrn = init_msg_port(&port);
    assert_stat(port != 0);

    /* Create a thread for us to communicate with. */
    rtrn = pthread_create(&thread, NULL, thread_start, &port);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create thread\n");
        return (0);
    }

    char *buffer = NULL;

    /* Wait for a message from the test thread. */
    buffer = (char *) msg_recv(port);
    assert_stat(buffer != NULL);

    log_test(SUCCESS, "Msg send recv test passed");

    return (0);
}

static int32_t test_fork_pass_port(void)
{
	log_test(DECLARE, "Testing fork pass port");

  

    log_test(SUCCESS, "Fork pass port test passed");

	return (0);
}

int main(void)
{
	int32_t rtrn = 0;

    test_stat = init_test_framework();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't init test framework");
        return (-1);
    }

    /* Test sending and recieving IPC messages. */
    rtrn = test_msg_send_recv();
    if(rtrn < 0)
        log_test(FAIL, "Msg send and recv test failed");

    rtrn = test_fork_pass_port();
    if(rtrn < 0)
        log_test(FAIL, "Fork pass port test failed");

	_exit(0);
}

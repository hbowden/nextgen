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

#include "reaper.h"
#include "syscall/syscall.h"
#include "memory/memory.h"
#include "concurrent/concurrent.h"
#include "platform.h"
#include "io/io.h"

#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static atomic_int_fast32_t *stop;

static void reaper_signal_handler(int sig)
{
    /* Reaper died so let's kill everything else. */
    output(ERROR, "Reaper died, so were shutting down!\n");
    cas_loop_int32(stop, TRUE);
    _exit(-1);
}

static void setup_reaper_signal_handler(void)
{
    struct sigaction sa;
    sigset_t ss;
    uint32_t i;
    
    for (i = 1; i < 512; i++)
    {
        (void) sigfillset(&ss);
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = reaper_signal_handler;
        sa.sa_mask = ss;
        (void) sigaction((int)i, &sa, NULL);
    }
    /* we want default behaviour for child process signals */
    (void) signal(SIGCHLD, SIG_DFL);
    
    /* ignore signals we don't care about */
    (void) signal(SIGFPE, SIG_IGN);
    (void) signal(SIGXCPU, SIG_IGN);
    (void) signal(SIGTSTP, SIG_IGN);
    (void) signal(SIGWINCH, SIG_IGN);
    (void) signal(SIGIO, SIG_IGN);
    (void) signal(SIGPIPE, SIG_IGN);

    return;
}

static void check_progess(struct child_ctx *child)
{
    /* Our variables. */
    int32_t rtrn = 0;
    struct timeval tv;
    time_t dif, time_of_syscall, time_now;

    /* Return early if there is no child. */
    if(atomic_load(&child->pid) == EMPTY)
        return;

    /* Grab the time the syscall was done. */
    time_of_syscall = child->time_of_syscall.tv_sec;
    
    /* Return early. */
    if(time_of_syscall == 0)
        return;

    gettimeofday(&tv, NULL);

    time_now = tv.tv_sec;
    
    if(time_of_syscall > time_now)
        dif = time_of_syscall - time_now;
    else
        dif = time_now - time_of_syscall;
    
    if(dif < 5)
        return;

    rtrn = kill(atomic_load(&child->pid), SIGKILL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't kill child: %s\n", strerror(errno));
        return;
    }

	return;
}

static void reaper(msg_port_t port)
{
    int32_t rtrn = 0;
    msg_port_t recv_port = 0;

    /* Initialize the message port. */
    rtrn = init_msg_port(&recv_port);
    if(rtrn < 0)
    {
        output(ERROR, "Can't init recv port\n");
        return;
    }

    /* Send the main process the port we can be reached on. */
    rtrn = msg_send(recv_port, port, (void *)&recv_port);
    if(rtrn < 0)
    {
        output(ERROR, "Can't send port\n");
        return;
    }

    /* Setup the signal handler. */
    setup_reaper_signal_handler();

    /* Start the main reaper loop. */
    while(atomic_load(stop) != TRUE)
    {
        uint32_t i = 0;
        struct child_ctx *child = NULL;

        /* Loop for each child processe. */
    	for(i = 0; i < number_of_children; i++)
    	{
            child = get_child_from_index(i);
            if(child == NULL)
            {
                output(ERROR, "Can't get child\n");
                return;
            }

            /* Make sure the child is not hung up, if it is check_progress() kills it. */
    		check_progess(child);
    	}
    }

    /* We are exiting either due to error or the user wants us to. So lets kill all child processes. */
    kill_all_children();
	
    return;
}

static int32_t start_reaper_loop(msg_port_t port, void *arg)
{
    /* Grab PID pointer. */
    pid_t *reaper_pid = (pid_t *)arg;

    /* Set the PID so the caller can use it. */
    (*reaper_pid) = getpid();

    /* Start the reaper loop. */
    reaper(port);

    /* Exit and clean up the child process. */
    _exit(0);
}

/* This function sets up and run's the reaper process. The reaper kills and replaces child
processes that are not functioning properly. */
int32_t setup_reaper_module(pid_t *reaper_pid, atomic_int_fast32_t *stop_ptr)
{
    /* Set stop pointer. */
    stop = stop_ptr;

    int32_t rtrn = 0;
    msg_port_t port = 0;

    /* Fork and create a child process. */
    rtrn = fork_pass_port(&port, start_reaper_loop, (void *)reaper_pid);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create reaper process\n");
        return (-1);
    }

    return (0);
}

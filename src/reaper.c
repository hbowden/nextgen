

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

#include "reaper.h"
#include "nextgen.h"
#include "signals.h"
#include "memory.h"
#include "concurrent.h"
#include "io.h"

#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static void check_progess(struct child_ctx *child)
{
    /* Our variables. */
    int rtrn;
    struct timeval tv;
    time_t dif, time_of_syscall, time_now;

    /* Return early if there is no child. */
    if(atomic_load(&child->pid) == EMPTY)
    {
        return;
    }

    /* Grab the time the syscall was done. */
    time_of_syscall = child->time_of_syscall.tv_sec;
    
    /* Return  */
    if(time_of_syscall == 0)
    {
        return;
    }

    gettimeofday(&tv, NULL);

    time_now = tv.tv_sec;
    
    if(time_of_syscall > time_now)
    {
        dif = time_of_syscall - time_now;
    }
    else
    {
        dif = time_now - time_of_syscall;
    }
    
    if(dif < 10)
    {
        return;
    }
    else
    {
        rtrn = kill(atomic_load(&child->pid), SIGKILL);
        if(rtrn < 0)
        {
            output(ERROR, "Can't kill child: %s\n", strerror(errno));
            return;
        }
    }

	return;
}

static void reaper(void)
{
    setup_reaper_signal_handler();

    while(atomic_load(&map->stop) != TRUE)
    {
    	struct child_ctx *child = NULL;

        unsigned int i = 0;

        /* Loop for each child processe. */
    	for_each_child(child, i)
    	{
            /* Make sure the child is not hung up, if it is kill it. */
    		check_progess(child);
    	}
    }

    /* We are exiting either due to error or the user wants us to. So lets kill all child processes. */
    kill_all_children();
	
    return;
}

/* This function sets up and run's the reaper process. The reaper kills and replaces child
processes that are not functioning properly. */
int setup_reaper_module(void)
{
    pid_t reaper_pid;

    reaper_pid = fork();
    if(reaper_pid == 0)
    {
        cas_loop_int32(&map->reaper_pid, reaper_pid);

    	reaper();
    }
    else if(reaper_pid > 0)
    {
    	return 0;
    }
    else
    {
    	output(ERROR, "Failed to fork reaper process: %s\n", strerror(errno));
    	return -1;
    }
	return 0;
}

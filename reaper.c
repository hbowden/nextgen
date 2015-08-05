

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

#include "nextgen.h"
#include "reaper.h"
#include "child.h"
#include "utils.h"

#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

static void check_progess(struct child_ctx *child)
{
    /* Our variables. */
    int rtrn;
    struct timeval tv;
    time_t dif, time_of_syscall, time_now;

    /* Return early if there is no child. */
    if(child->pid == EMPTY)
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
        rtrn = kill(child->pid, SIGKILL);
        if(rtrn < 0)
        {
            output(ERROR, "Can't kill child: %s\n", strerror(errno));
            return;
        }
    }

	return;
}

static void reap_child(struct child_ctx *child)
{

    return;
}

static void reap_dead_children(void)
{
    int rtrn;
    unsigned int i;
    unsigned int number_of_children = map->number_of_children;

    for(i = 0; i < number_of_children; i++)
    {
        if(map->children[i]->pid == EMPTY)
        {
            continue;
        }

        rtrn = kill(map->children[i]->pid, 0);
        if(rtrn < 0)
        {
            if(errno == ESRCH)
            {
                reap_child(map->children[i]);
            }
        }

        if(map->running_children == 0)
        {
            return;
        }

        map->children[i]->pid = EMPTY;
    }
}

static void kill_all_children(void)
{
    int rtrn;
    unsigned int i;
    unsigned int number_of_children = map->number_of_children;

    for(i = 0; i < number_of_children; i++)
    {
        rtrn = kill(map->children[i]->pid, SIGKILL);
        if(rtrn < 0)
        {
            output(ERROR, "Can't kill child: %s\n", strerror(errno));
            return;
        }
    }
}

static void reaper(void)
{
    while(atomic_load(&map->stop) == FALSE)
    {
    	unsigned int i;

    	reap_dead_children();

    	unsigned int number_of_children = map->number_of_children;

    	for(i = 0; i <number_of_children; i++)
    	{
    		check_progess(map->children[i]);


    	}
    }

    /* We are exiting either due to error or the user wants us to. So lets kill all child processes. */
    kill_all_children();
	
    return;
}

/* This function sets up and run's the reaper process. The reaper kills and replaces child
processes that are not functioning properly. */
int setup_and_run_reaper(void)
{
    map->reaper_pid = fork();
    if(map->reaper_pid == 0)
    {
    	reaper();
    }
    else if(map->reaper_pid > 0)
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
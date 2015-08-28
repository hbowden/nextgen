

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
#include "utils.h"

#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

int add_args_to_cleanup_list(struct child_ctx *ctx)
{
    int rtrn;
    unsigned int i;
    unsigned int number_of_args = ctx->number_of_args;

    for(i = 0; i < number_of_args; i++)
    {
        struct list_node *node;
        struct list_data *data;

        node = malloc(sizeof(struct list_node));
        if(node == NULL)
        {
            output(ERROR, "Can't malloc new list_node: %s\n", strerror(errno));
            return -1;
        }

        data = malloc(sizeof(struct list_data));
        if(data == NULL)
        {
            output(ERROR, "Can't malloc list data: %s\n", strerror(errno));
            free(node);
            return -1;
        }

        data->arg_value = malloc(ctx->arg_size_index[i] + 1);
        if(data->arg_value == NULL)
        {
            output(ERROR, "Can't malloc arg value: %s\n", strerror(errno));
            free(node);
            free(data);
            return -1;
        }

        data->arg_type = ctx->arg_type_index[i];

        memcpy(data->arg_value, ctx->arg_value_index[i], ctx->arg_size_index[i]);
    }
    
    return 0;
}

static int clean_list(struct child_ctx *ctx)
{
    
    return 0;
}

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

static void reap_child(struct child_ctx *child)
{
    compare_and_swap_int32(&child->pid, EMPTY);

    child->time_of_syscall.tv_sec = 0;

    return;
}

static void reap_dead_children(void)
{
    int rtrn;
    unsigned int i;
    unsigned int number_of_children = map->number_of_children;

    for(i = 0; i < number_of_children; i++)
    {
        if(atomic_load(&map->children[i]->pid) == EMPTY)
        {
            continue;
        }

        output(STD, "Killing\n");

        rtrn = kill(atomic_load(&map->children[i]->pid), 0);
        if(rtrn < 0)
        {
            if(errno == ESRCH)
            {
                reap_child(map->children[i]);
            }
        }

        if(atomic_load(&map->running_children) == 0)
        {
            return;
        }

        compare_and_swap_int32(&map->children[i]->pid, EMPTY);
    }
}

static void kill_all_children(void)
{
    int rtrn;
    unsigned int i;
    unsigned int number_of_children = map->number_of_children;

    for(i = 0; i < number_of_children; i++)
    {
        rtrn = kill(atomic_load(&map->children[i]->pid), SIGKILL);
        if(rtrn < 0)
        {
            output(ERROR, "Can't kill child: %s\n", strerror(errno));
            return;
        }
    }
}

static void reaper(void)
{
    while(atomic_load(&map->stop) != TRUE)
    {
    	unsigned int i;
    	unsigned int number_of_children = map->number_of_children;

        /* Loop for each child processe. */
    	for(i = 0; i < number_of_children; i++)
    	{
            /* Make sure the child is not hung up, if it is kill it. */
    		check_progess(map->children[i]);

            /* Clean up the child's resource list. */
            clean_list(map->children[i]);
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
    pid_t reaper_pid;

    reaper_pid = fork();
    if(reaper_pid == 0)
    {
        compare_and_swap_int32(&map->reaper_pid, reaper_pid);

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


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
#include "utils.h"

#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ck_queue.h>

int add_pid_to_list(pid_t pid, struct child_ctx *ctx)
{
    /* Our variables. */
    int rtrn;
    struct list_node *node;

    /* Set pointer to free space in list_node memory pool. 
    rtrn = create_list_node(&node, ctx);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create list node\n");
        return -1;
    } */

    /* Set argument type. */
    node->data->arg_type = PID;

    /* Copy arg value to the list node. */
    node->data->pid = pid;

    /* Set the time we created this node. */
    gettimeofday(&node->data->create_time, NULL);

    /* Insert the node in the list. */
    CK_LIST_INSERT_HEAD(&ctx->list, node, list_entry);

    return 0;
}

int add_socket_to_list(int socket_fd, struct child_ctx *ctx)
{
    /* Our variables. */
    int rtrn;
    struct list_node *node;

    /* Set pointer to free space in list_node memory pool. 
    rtrn = create_list_node(&node, ctx);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create list node\n");
        return -1;
    } */

    /* Set argument type. */
    node->data->arg_type = SOCKET;

    /* Copy arg value to the list node. */
    node->data->socket_or_fd = socket_fd;

    /* Set the time we created this node. */
    gettimeofday(&node->data->create_time, NULL);

    /* Insert the node in the list. */
    CK_LIST_INSERT_HEAD(&ctx->list, node, list_entry);

    return 0;
}

int add_path_to_list(char *path, struct child_ctx *ctx)
{
    /* Our variables. */
    int rtrn;
    struct list_node *node;

    printf("node: %p\n", node);

    /* Set pointer to free space in list_node memory pool. 
    rtrn = create_list_node(&node, ctx);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create list node\n");
        return -1;
    } */

    /* Set argument type. */
    node->data->arg_type = FILE_PATH;

    node->data->node_number = rtrn;

    /* Copy arg value to the list node. */
    rtrn = asprintf(&node->data->path, "%s", path);
    if(rtrn < 0)
    {
        output(ERROR, "Can't copy path: %s\n", path);
        return -1;
    }

    /* Set the time we created this node. */
    gettimeofday(&node->data->create_time, NULL);

    /* Insert the node in the list. */
    CK_LIST_INSERT_HEAD(&ctx->list, node, list_entry);

    return 0;
}

static int clean_list(struct child_ctx *ctx)
{
    int rtrn;
    unsigned int limit = 8;
    unsigned int nodes_cleaned = 0;
    struct list_node *node;

    /* Check if the arg cleanup list is empty, if it is empty return early. */
    if(CK_LIST_EMPTY(&ctx->list) == TRUE)
    {
        return 0;
    }
   
    /* Loop for each node in the list. */
    CK_LIST_FOREACH(node, &ctx->list, list_entry)
    {
        struct timeval tv;
        time_t dif, time_now;

        /* Grab current time.*/
        gettimeofday(&tv, NULL);
        time_now = tv.tv_sec;
    
        if(node->data->create_time.tv_sec > time_now)
        {
            dif = node->data->create_time.tv_sec - time_now;
        }
        else
        {
            dif = time_now - node->data->create_time.tv_sec;
        }
    
        if(dif < 5)
        {
            continue;
        }

        switch(node->data->arg_type)
        {
            case FILE_PATH:
                rtrn = remove(node->data->path);
                if(rtrn < 0)
                {
                    output(ERROR, "Can't remove file\n");
                    return -1;
                }
                output(STD, "Removed path\n");
                break;

            case SOCKET:
                rtrn = close(node->data->socket_or_fd);
                if(rtrn < 0)
                {
                    output(ERROR, "Can't close socket\n");
                    return -1;
                }
                output(STD, "Closed socket\n");
                break;


            case PID:
                rtrn = kill(node->data->pid, 0);
                if(rtrn < 0)
                {
                    output(ERROR, "Can't kill temp child\n");
                    return -1;
                }
                output(STD, "killed child proc\n");
                break;

            default:
                output(ERROR, "Unknown list arg type: %d\n", node->data->arg_type);
                return -1;
        }


        CK_LIST_REMOVE(node, list_entry);

        switch(node->data->node_number)
        {
            case 0:
                compare_and_swap_bool(&ctx->pool->slot1_full, FALSE);
                break;

            case 1:
                compare_and_swap_bool(&ctx->pool->slot2_full, FALSE);
                break;

            case 2:
                compare_and_swap_bool(&ctx->pool->slot3_full, FALSE);
                break;

            case 3:
                compare_and_swap_bool(&ctx->pool->slot4_full, FALSE);
                break;

            case 4:
                compare_and_swap_bool(&ctx->pool->slot5_full, FALSE);
                break;

            case 5:
                compare_and_swap_bool(&ctx->pool->slot6_full, FALSE);
                break;

            case 6:
                compare_and_swap_bool(&ctx->pool->slot7_full, FALSE);
                break;

            case 7:
                compare_and_swap_bool(&ctx->pool->slot8_full, FALSE);
                break;

            default:
                output(ERROR, "Unknown slot number: %d\n", node->data->node_number);
                return -1;
        }

        nodes_cleaned++;

        if(nodes_cleaned <= limit)
        {
            break;
        }

        output(STD, "Cleaning\n");
    }

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
    setup_reaper_signal_handler();

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
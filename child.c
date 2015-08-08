

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

#include "child.h"
#include "utils.h"
#include "crypto.h"
#include "syscall.h"
#include "signal.h"
#include "nextgen.h"

#include <string.h>
#include <errno.h>

static int get_child_syscall_table(struct child_ctx *child)
{
    
    return 0;
}

static int init_syscall_child(struct child_ctx *child)
{
    int rtrn;

    /* Set the pid of the child. */
    child->pid = getpid();

    /* Set up the child signal handler. */
    setup_syscall_child_signal_handler();

    /* Grab a pointer to the syscall table and store it in the child context object. */
    rtrn = get_child_syscall_table(child);
    if(rtrn < 0)
    {
        output(ERROR, "Can't init syscall \n");
        return -1;
    }

    /* We got to seed the prng so that the child process trys different syscalls. */
    rtrn = seed_prng();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init syscall \n");
        return -1;
    }

    map->running_children++;

    return 0;
}

/* This function returns a pointer to the childs context object. */
struct child_ctx *get_child_ctx(void)
{
    struct child_ctx *ctx;

    unsigned int i;
    unsigned int number_of_children = map->number_of_children;

    int pid = getpid();

    for(i = 0; i < number_of_children; i++)
    {
        if(map->children[i]->pid == pid)
        {
            ctx = map->children[i];

            return ctx;
        }
    }

    /* Should not get here. */
    return NULL;
}

static void start_file_child(void)
{
	return;
}

static void exit_child(void)
{
    map->running_children--;
    _exit(0);
}

/**
 * This is the fuzzing loop for syscall fuzzing mode.
 */
static void start_syscall_child(void)
{
    int rtrn;

    /* grab the child_ctx struct for this child process. */
    struct child_ctx *ctx = get_child_ctx();
    if(ctx == NULL)
    {
        output(ERROR, "Can't grab child context\n");
        exit_child();
    }

    /* Set the return jump so that we can try fuzzing again on a signal. */
    rtrn = setjmp(ctx->return_jump);
    if(rtrn < 0)
    {
        output(ERROR, "Can't set return jump\n");
        exit_child();
    }

    /* Check if we should stop or continue running. */
    while(atomic_load(&map->stop) != TRUE)
    {
        sleep(4); // Temp until ctrl-c bug is fixed.

        /* Randomly pick the syscall to test. */
        rtrn = pick_syscall(ctx);
        if(rtrn < 0)
        {
            output(ERROR, "Can't pick syscall to test\n");
            exit_child();
        }

        rtrn = generate_arguments(ctx);
        if(rtrn < 0)
        {
            output(ERROR, "Can't pick syscall to test\n");
            exit_child();
        }

    }

    exit_child();
}

void create_syscall_children(void)
{
	/* Walk the child structure and find the first empty child slot. */
	unsigned int i;
	unsigned int number_of_children = map->number_of_children;

    for(i = 0; i < number_of_children; i++)
    {
        /* If the child has a pid of EMPTY let's create a new one. */
        if(map->children[i]->pid == EMPTY)
        {
            map->children[i]->pid = fork();
            if(map->children[i]->pid == 0)
            {
                /* Initialize the new syscall child. */
                init_syscall_child(map->children[i]);

                /* Start the child main loop. */
                start_syscall_child();
            }
            else if(map->children[i]->pid > 0)
            {
                 /* This is the parent process, so let's keep looping. */
            
            }
            else
            {
                output(ERROR, "Can't create child process: %s\n", strerror(errno));
                return;
            }
        }
    }
	return;
}

void manage_syscall_children(void)
{
	return;
}

void create_file_children(void)
{
	/* Walk the child structure and find the first empty child slot. */
	unsigned int i;
	unsigned int number_of_children = map->number_of_children;

	for(i = 0; i < number_of_children; i++)
	{
		/* If the child has a pid of EMPTY let's create a new one. */
		if(map->children[i]->pid == EMPTY)
		{
            map->children[i]->pid = fork();
            if(map->runloop_pid == 0)
            {
            	/* Start the child main loop. */
                start_file_child();
            }
            else if(map->runloop_pid > 0)
            {
               /* This is the parent process, so let's keep looping. */
                continue;
            
            }
            else
            {
                output(ERROR, "Can't create child process: %s\n", strerror(errno));
                return;
            }
		}
	}
	return;
}

void manage_file_children(void)
{
	return;
}

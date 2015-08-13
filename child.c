

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

static int init_syscall_child(unsigned int i)
{
    int rtrn;

    /* Get the pid of the child. */
    pid_t child_pid = getpid();

    /* Set the child pid. */
    compare_and_swap_int32(&map->children[i]->pid, child_pid);

    /* Set up the child signal handler. */
    setup_syscall_child_signal_handler();

    /* We got to seed the prng so that the child process trys different syscalls. */
    rtrn = seed_prng();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init syscall \n");
        return -1;
    }

    /* Increment the running child counter. */
    atomic_fetch_add(&map->running_children, 1);

    /* Inform main loop we are done setting up. */
    write(map->children[i]->msg_port[1], "1", 1);

    return 0;
}

/* This function returns a pointer to the childs context object. */
int get_child_index_number(void)
{
    unsigned int i;
    unsigned int number_of_children = map->number_of_children;

    int pid = getpid();

    for(i = 0; i < number_of_children; i++)
    {
        if(atomic_load(&map->children[i]->pid) == pid)
        {
            return i;
        }
    }
    /* Should not get here. */
    return -1;
}

static void start_file_child(void)
{
	return;
}

static void exit_child(void)
{
    /* Decrement the running child counter and exit. */
    atomic_fetch_sub(&map->running_children, 1);
    _exit(0);
}

/**
 * This is the fuzzing loop for syscall fuzzing mode.
 */
static void start_syscall_child(void)
{
    int rtrn;
    int child_number;

    /* grab the child_ctx struct for this child process. */
    child_number = get_child_index_number();
    if(child_number < 0)
    {
        output(ERROR, "Can't grab child number\n");
        exit_child();
    }

    struct child_ctx *ctx = map->children[child_number];

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
    int rtrn;

	/* Walk the child structure and find the first empty child slot. */
	unsigned int i;
	unsigned int number_of_children = map->number_of_children;

    for(i = 0; i < number_of_children; i++)
    {
        /* If the child has a pid of EMPTY let's create a new one. */
        if(atomic_load(&map->children[i]->pid) == EMPTY)
        {
            pid_t child_pid;

             /* Create pipe here so we can avoid a race condition. */
            rtrn = pipe(map->children[i]->msg_port);
            if(rtrn < 0)
            {
                output(ERROR, "Can't create msg port: %s\n", strerror(errno));
                return;
            }

            child_pid = fork();
            if(child_pid == 0)
            {
                /* Initialize the new syscall child. */
                init_syscall_child(i);

                /* Start the child main loop. */
                start_syscall_child();
            }
            else if(child_pid > 0)
            {
                char *msg_buf;

                /* Wait for the child to be done setting up. */
                size_t ret = read(map->children[i]->msg_port[0], msg_buf, 1);
                if(ret < 1)
                {
                    output(ERROR, "Problem waiting for child setup: %s\n", strerror(errno));
                    return;
                }
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
		if(atomic_load(&map->children[i]->pid) == EMPTY)
		{
            pid_t child_pid;

            child_pid = fork();
            if(child_pid == 0)
            {
                compare_and_swap_int32(&map->children[i]->pid, child_pid);

            	/* Start the child main loop. */
                start_file_child();
            }
            else if(child_pid > 0)
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



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

#include "runtime.h"
#include "child.h"
#include "utils.h"
#include "crypto.h"
#include "disas.h"
#include "probe.h"
#include "reaper.h"
#include "nextgen.h"

#include <stdint.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

static void ctrlc_handler(int sig)
{

    sig = 0; // We do this to temprorarly silence a unused parameter warning.
    shutdown();
    
    return;
}

static void start_main_syscall_loop(void)
{
    return;
}

static void start_main_file_loop(void)
{
    /* Check if we should stop or continue running. */
    while(atomic_load(&map->stop) == FALSE)
    {
        /* Check if we have the right number of children processes running, if not create a new ones until we do. */
        if(map->running_children < map->number_of_children)
        {
            /* Create children process. */
            create_children();

            /* Make sure the child struct is setup properly. */
            manage_children();
        }
        
        output(STD, "Exiting main loop\n");
    }

    return;
}

static void setup_signal_handler(void)
{
    (void) signal(SIGINT, ctrlc_handler);

    return;
}

static int start_network_mode_runtime(void)
{
    return 0;
}

static int start_syscall_mode_runtime(void)
{
    map->runloop_pid = fork();
    if(map->runloop_pid == 0)
    {
        start_main_syscall_loop();
    }
    else if(map->runloop_pid > 0)
    {
        int status;

        waitpid(map->runloop_pid, &status, 0);

        waitpid(map->reaper_pid, &status, 0);

        output(STD, "Exiting\n");

        return 0;
    }
    else
    {
        output(ERROR, "runloop fork failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

static int start_file_mode_runtime(void)
{
    map->runloop_pid = fork();
    if(map->runloop_pid == 0)
    {
        start_main_file_loop();
    }
    else if(map->runloop_pid > 0)
    {
        int status;

        waitpid(map->runloop_pid, &status, 0);

        waitpid(map->reaper_pid, &status, 0);

        output(STD, "Exiting\n");

        return 0;
    }
    else
    {
        output(ERROR, "runloop fork failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

static int setup_file_mode_runtime(void)
{
    /* Lets parse the binary and figure out the virtual memory address it's going to be loaded at. */
    get_load_address();

    /* Announce address we will be injecting code into. */
    output(STD, "Target executable's start address: Ox%x\n", map->exec_ctx->main_start_address);

     /* Now inject dtrace probes into the target process. We will use these probes to calculate
     the code coverage of fuzzing proccess. We must do this before injecting the fork server so we can
     avoid injecting probes on each fork(). */
    inject_probes();
    
    /* Inject the fork server. One can learn more about the fork server idea at:
    http://lcamtuf.blogspot.com/2014/10/fuzzing-binaries-without-execve.html .
    We use the fork server so that we can avoid the cost of dtrace probe injection and execv
    calls on each fuzz test. This implementation of the fork server use's ptrace as opposed to
    a custom C/C++ compiler like the orignal implementation. */
    inject_fork_server();

    /* Lets set up the signal handler for the main process. */
    setup_signal_handler();

    return 0;
}

static int setup_network_mode_runtime(void)
{
    return 0;
}

static int setup_syscall_mode_runtime(void)
{
    return 0;
}

int setup_runtime(void)
{
    output(STD, "Setting up fuzzer\n");

    /* Here we do work that's common across different fuzzing modes, 
     then we call the init function for the selected fuzzing. */
    int rtrn;

    /* This function sets up the other crypto functions and crypto library.  */
    setup_crypto();

    /* Now let's start the reaper process, so it can clean up misbehaving processes. */
    rtrn = setup_and_run_reaper();
    if(rtrn < 0)
    {
        output(ERROR, "Can't set up the reaper\n");
        return -1;
    }

    /* We are done doing common init work now we call the specific init routines. */ 
    switch((int)map->mode)
    {
        case MODE_FILE:
            setup_file_mode_runtime();
            break;

        case MODE_NETWORK:
            setup_network_mode_runtime();
            break;

        case MODE_SYSCALL:
            setup_syscall_mode_runtime();
            break;

        default:
            output(ERROR, "Unknown fuzzing mode\n");
            return -1;
    }

    return 0;
}

int start_runtime(void)
{
    output(STD, "Starting fuzzer\n");

     /* Start the selected fuzzer runtime. */ 
    switch((int)map->mode)
    {
        case MODE_FILE:
            start_file_mode_runtime();
            break;

        case MODE_NETWORK:
            start_network_mode_runtime();
            break;

        case MODE_SYSCALL:
            start_syscall_mode_runtime();
            break;

        default:
            output(ERROR, "Don't understand fuzzing mode selected.\n");
            return -1;
    }

    return 0;
}

int shutdown(void)
{
    /* Set the atomic flag so that other processes know to start their shutdown procedure. */
    compare_and_swap_loop(&map->stop, TRUE);

    return 0;
}
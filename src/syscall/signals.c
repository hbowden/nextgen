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

#include "signals.h"
#include "io/io.h"
#include "runtime/nextgen.h"
#include "runtime/runtime.h"
#include "syscall/context.h"
#include "syscall/syscall.h"

#include <signal.h>

static void ctrlc_handler(int sig)
{
    (void)sig;
    shutdown();
    return;
}

void setup_signal_handler(void)
{
    (void)signal(SIGFPE, SIG_DFL);
    (void)signal(SIGCHLD, SIG_DFL);
    (void)signal(SIGINT, ctrlc_handler);

    return;
}

static void syscall_child_signal_handler(int sig)
{

    /* Grab our child's context object. */
    struct child_ctx *child = get_child_ctx();
    if(child == NULL)
    {
        output(ERROR, "Can't get child context\n");
        return;
    }

    /* Check what kind of signal got us here. */
    switch(sig)
    {
        /* An alarm we set for a blocking syscall has gone off. */
        case SIGALRM:

            /* Set the had error flag to no. */
            child->had_error = NX_NO;

            /* Set return value to zero. */
            child->ret_value = 0;
            
            /* Jump back to child's main loop. */
            longjmp(child->return_jump, 1); 

        case SIGSEGV:

            /* Set the had error flag to YES. */
            child->had_error = NX_YES;

            /* Set return value to -1. */
            child->ret_value = -1;

            /* Set err value. */
            memmove(child->err_value, "SIGSEGV", 7);

            child->err_value[7] = '\0';

            /* Jump back to child's main loop. */
            longjmp(child->return_jump, 1);

        default:

            /* Set the had error flag to YES. */
            child->had_error = NX_YES;

            /* Set return value to -1. */
            child->ret_value = -1;

            /* Set err value. */
            memmove(child->err_value, "SIGNAL", 6);

            child->err_value[6] = '\0';

            /* Jump back to child's main loop. */
            longjmp(child->return_jump, 1);
    }
}

void setup_syscall_child_signal_handler(void)
{
    struct sigaction sa;
    sigset_t ss;
    unsigned int i;

    for(i = 1; i < 512; i++)
    {
        (void)sigfillset(&ss);
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = syscall_child_signal_handler;
        sa.sa_mask = ss;
        (void)sigaction((int)i, &sa, NULL);
    }
    /* we want default behaviour for child process signals */
    (void)signal(SIGCHLD, SIG_DFL);

    /* ignore signals we don't care about */
    (void)signal(SIGFPE, SIG_IGN);
    (void)signal(SIGXCPU, SIG_IGN);
    (void)signal(SIGTSTP, SIG_IGN);
    (void)signal(SIGWINCH, SIG_IGN);
    (void)signal(SIGIO, SIG_IGN);
    (void)signal(SIGPIPE, SIG_IGN);

    /* trap ctrl-c */
    (void)signal(SIGINT, ctrlc_handler);

    return;
}

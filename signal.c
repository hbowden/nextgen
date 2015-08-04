

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
#include "runtime.h"
#include "signal.h"

#include <signal.h>

static void ctrlc_handler(int sig)
{

    sig = 0; // We do this to temprorarly silence a unused parameter warning.
    shutdown();
    
    return;
}

void setup_signal_handler(void)
{
    (void) signal(SIGINT, ctrlc_handler);

    return;
}

static void syscall_child_signal_handler(int sig)
{
    struct child_ctx *child = get_child_ctx();
    if(child == NULL)
    {
        _exit(0);
    }

    switch(sig)
    {
    	case SIGALRM:

            alarm(1);

            (void) signal(sig, syscall_child_signal_handler);

            longjmp(child->return_jump, 1);

            /* No break needed, as we are jumping back to the child syscall fuzzing loop. */


    	default:
            _exit(0);

    }

	return;
}

void setup_syscall_child_signal_handler(void)
{
    struct sigaction sa;
    sigset_t ss;
    unsigned int i;
    
    for (i = 1; i < 512; i++)
    {
        (void) sigfillset(&ss);
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = syscall_child_signal_handler;
        sa.sa_mask = ss;
        (void) sigaction(i, &sa, NULL);
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
    
    /* trap ctrl-c */
    (void) signal(SIGINT, ctrlc_handler);

	return;
}

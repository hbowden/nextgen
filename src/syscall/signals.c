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
#include "syscall/syscall.h"

#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <setjmp.h>

static void child_exit_handler(int sig)
{
    pid_t pid = 0;
    int32_t status = 0;

    while((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if(WIFEXITED(status))
        {
            struct child_ctx *child = NULL;

            child = get_child_ctx_from_pid(pid);
            if(child == NULL)
            {
                output(ERROR, "Can't get child context\n");
                return;
            }

            set_child_pid(child, EMPTY);
        }
    }

    return;
}

static void ctrlc_handler(int sig)
{
    (void)sig;
    shutdown();
    return;
}

static int32_t setup_ctrlc_handler(void)
{
    int32_t rtrn = 0;
    struct sigaction sa;
    
    /* Set the SIGINT or ctrlc signal handling function. */
    sa.sa_handler = &ctrlc_handler;

    /* Block every signal during the handler */
    sigfillset(&sa.sa_mask);

    /* Trap ctrl-c */
    rtrn = sigaction(SIGINT, &sa, NULL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup SIGINT handler: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

int32_t setup_signal_handler(void)
{
    int32_t rtrn = 0;

    rtrn = setup_ctrlc_handler();
    if(rtrn < 0)
    {
        output(ERROR, "Can't handle SIGINT\n");
        return (-1);
    }

    struct sigaction sa;
    
    /* Set the child exit handler. */
    sa.sa_handler = &child_exit_handler;

    /* Block every signal during the handler */
    sigfillset(&sa.sa_mask);

    /* Trap SIGCHLD signals */
    rtrn = sigaction(SIGCHLD, &sa, NULL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup SIGCHLD handler: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

static void child_signal_handler(int sig)
{
    /* Grab our child's context object. */
    struct child_ctx *child = get_child();
    if(child == NULL)
    {
        output(ERROR, "Can't get child context\n");
        return;
    }

    //epoch_begin(&child->record, NULL);

    /* Check what kind of signal got us here. */
    switch(sig)
    {
        /* An alarm we set for a blocking syscall has gone off. */
        case SIGALRM:
            //child->had_error = NX_NO;
            //child->ret_value = 0;
            //child->did_jump = NX_YES;
            break;

        default:
            //child->had_error = NX_YES;
            //child->ret_value = -1;
            //child->did_jump = NX_YES;
            //child->sig_num = sig;
            break;
    }

    jmp_buf jmp;
    get_return_jump(child, &jmp);

    //epoch_end(&child->record, NULL);

    /* Jump back to child's main loop. */
    longjmp(jmp, 1);
}

int32_t setup_child_signal_handler(void)
{
    int32_t rtrn = 0;

    rtrn = setup_ctrlc_handler();
    if(rtrn < 0)
    {
        output(ERROR, "Can't handle SIGINT\n");
        return (-1);
    }

    struct sigaction sa;
    
    /* Set the child signal handler. */
    sa.sa_handler = &child_signal_handler;

    /* Block every signal during the handler */
    sigfillset(&sa.sa_mask);

    rtrn = sigaction(SIGBUS, &sa, NULL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup SIGBUS handler: %s\n", strerror(errno));
        return (-1);
    }

    rtrn = sigaction(SIGSEGV, &sa, NULL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup SIGSEGV handler: %s\n", strerror(errno));
        return (-1);
    }

    rtrn = sigaction(SIGALRM, &sa, NULL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup SIGALRM handler: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

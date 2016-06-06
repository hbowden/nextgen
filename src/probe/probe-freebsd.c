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

#include "probe.h"
#include "utils/utils.h"
#include "io/io.h"

#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <machine/reg.h>

static pid_t pid;

int32_t start_and_pause_target(char *exec_path, pid_t *target_pid)
{
    int32_t rtrn = 0;
    char * const argv[] = {"blah", NULL};

    /* Create child process. */
    (*target_pid) = fork();
    if((*target_pid) == 0)
    {
        /* Get our PID. */
        (*target_pid) = getpid();

        /* Drop root privileges. */
        rtrn = drop_privileges();
        if(rtrn < 0)
        {
            output(ERROR, "Can't drop privileges\n");
            return (-1);
        }
        /* Let's announce we want to be traced so that we don't execute any instructions on execv. */
        ptrace(PT_TRACE_ME, 0, NULL, 0);

        /* Now we execute the target binary. */
        rtrn = execv(exec_path, argv);
        if(rtrn < 0)
        {
            output(ERROR, "execv: %s\n", errno);
            return -1;
        }

        _exit(0);
    }
    else if((*target_pid) > 0)
    {
        int32_t status = 0;

        /* Wait until the target binary has stopped. */
        wait4((*target_pid), &status, 0, NULL);

        /* Lets check the reason why were not waiting anymore. Hopefully it's
         because the target executable is stopped. */

        if(WIFCONTINUED(status) != 0)
        {
            output(ERROR, "The process we are testing is continuing?\n");
            return -1;
        }

        /* Check if the process exited. */
        if(WIFEXITED(status) != 0)
        {
            output(ERROR, "The process we are testing has exited prematurely\n");
            return -1;
        }

        /* Check if the process was terminated due to a signal. */
        if(WIFSIGNALED(status) != 0)
        {
            output(ERROR, "The process we are testing has exited due to a signal\n");
            return -1;
        }

        /* Check if the process exited stopped due to ptrace. This is the Macro that we hope
        evaluates to true. */
        if(WIFSTOPPED(status) != 0)
            goto EXIT;

        /* We should not get here. */
        return -1;
    }

EXIT:

    pid = (*target_pid);

    return 0;
}

int32_t inject_fork_server(uint64_t main_address)
{
    output(STD, "Creating fork server\n");

    /* Variables. */
    struct reg regs;
    int32_t orig = 0;
    int32_t status = 0;
    uint64_t start_offset = 0;
        
    /* Lets save the code at main in the target process. */
    orig = ptrace(PT_READ_I, pid, (caddr_t)&start_offset, 0);

   /* start_offset = get_start_addr();

     Let's set a breakpoint on main. 
    ptrace(PT_WRITE_I, pid, (caddr_t)&start_offset, (orig & TRAP_MASK) | TRAP_INST);

    Now we continue until the breakpoint. 
    ptrace(PT_CONTINUE, pid, (caddr_t)1, 0); 

     Wait until the breakpoint is hit.  */
    waitpid(pid, &status, 0);

    if(WIFCONTINUED(status) != 0)
    {
        output(ERROR, "The process we are testing is continuing?\n");
        return -1;
    }

    /* Check if the process exited. */
    if(WIFEXITED(status) != 0)
    {
        output(ERROR, "The process we are testing has exited prematurely\n");
        return -1;
    }

    /* Check if the process was terminated due to a signal. */
    if(WIFSIGNALED(status) != 0)
    {
        output(ERROR, "The process we are testing has exited due to a signal\n");
        return -1;
    }

     /* Check if the process exited stopped due to ptrace. This is the Macro that we hope
    evaluates to true. */
    if(WIFSTOPPED(status) != 0)
    {
        
    }

    /* Lets grab the registers at main/breakpoint .*/
    ptrace(PT_GETREGS, pid, (caddr_t)&regs, 0);

    printf("rax: Ox%lx\n", regs.r_rax);

    return 0;
}


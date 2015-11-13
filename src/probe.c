

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

#include "probe.h"
#include "disas.h"
#include "concurrent.h"
#include "capstone.h"
#include "io.h"
#include "shim.h"

#include <inttypes.h>
#include <dtrace.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>

/* As the name implies this is a handle used for dtrace. */
static dtrace_hdl_t *dtrace_handle;

static dtrace_proginfo_t info;

static dtrace_prog_t* prog;

static const char *target_path;

static char *const args[] = { NULL, NULL };

static pid_t target_pid;

int inject_kernel_probes(void)
{
    
    return 0;
}

int inject_fork_server(void)
{
    return _inject_fork_server();
}

int start_and_pause_target(void)
{
    int32_t rtrn;

    /* Create child process. */
    target_pid = fork();
    if(target_pid == 0)
    {
        target_pid = getpid();

        /* Drop root privileges some programs don't like being run as root. */
        rtrn = setgid(10);
        if(rtrn < 0)
        {
            output(ERROR, "Can't drop privileges: %s\n", strerror(errno));
            return -1;
        }

        rtrn = setuid(10);
        if(rtrn < 0)
        {
            output(ERROR, "Can't drop uid: %s\n", strerror(errno));
            return -1;
        }

        /* Let's announce we want to be traced so that we don't execute any instructions on execv. */
        ptrace(PT_TRACE_ME, 0, NULL, 0);

        /* Now we execute the target binary. */
        rtrn = execv(target_path, args);
        if(rtrn < 0)
        {
            output(ERROR, "execv: %s\n", errno);
            return -1;
        }

        _exit(0);
    }
    else if(target_pid > 0)
    {
        int status;

        /* Wait until the target binary has stopped. */
        wait4(target_pid, &status, 0, NULL);

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
    return 0;
}

int inject_probes(void)
{
    int rtrn = 0;
    char *dtrace_prog = NULL;

    /* Create dtrace script that injects all probes into a target process. */
    rtrn = asprintf(&dtrace_prog, "pid%d:::", target_pid);
    if(rtrn < 0)
    {
       output(ERROR, "Can't create probe program string\n");
       return -1;
    }

    /* Create a dtrace handle. */
    dtrace_handle = dtrace_open(DTRACE_VERSION, 0, &rtrn);
    if(dtrace_handle == NULL)
    {
       fprintf(stderr, "failed to initialize dtrace: %s\n", dtrace_errmsg(NULL, rtrn));
       return -1;
    }

    /* Set dtrace options. */
    (void) dtrace_setopt(dtrace_handle, "bufsize", "4m");
    (void) dtrace_setopt(dtrace_handle, "aggsize", "4m");

    /* Compile the dtrace program. */
    prog = dtrace_program_strcompile(dtrace_handle, dtrace_prog, DTRACE_PROBESPEC_NAME, 0, 0, NULL);
    if(prog == NULL)
    {
       output(ERROR, "failed to compile dtrace program\n");
       return -1;
    }

    /* Send the dtrace program to the kernel and execute it. */
    rtrn = dtrace_program_exec(dtrace_handle, prog, &info);
    if(rtrn < 0)
    {
       output(ERROR, "failed to enable dtrace probes\n");
       return -1;
    }

    /* Inject and start the probes. */
    rtrn = dtrace_go(dtrace_handle);
    if(rtrn < 0)
    {
        output(ERROR, "Can't start probes\n");
        return -1;
    }

    /* Sleep so we don't record any information. */
    dtrace_sleep(dtrace_handle);

    return 0;
}

int setup_probe_module(char *exec_path)
{
    /* Set the path to the target executable. */
    target_path = exec_path;

    return 0;
}

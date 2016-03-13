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
#include "concurrent/concurrent.h"
#include "disas/disas.h"
#include "io/io.h"

#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

static dtrace_hdl_t *dtrace_handle;

static dtrace_proginfo_t info;

static dtrace_prog_t *prog;

static const char *target_path;

static pid_t target_pid;

int32_t inject_kernel_probes(dtrace_hdl_t *handle) { return (0); }

int32_t cleanup_kernel_probes(dtrace_hdl_t *handle) { return (0); }

int32_t inject_probes(pid_t pid)
{
    int32_t rtrn = 0;
    char *dtrace_prog = NULL;

    output(STD, "Injecting probes\n");

    /* Create dtrace script that injects all probes into a target process. */
    rtrn = asprintf(&dtrace_prog, "pid%d:::", pid);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create probe program string\n");
        return (-1);
    }

    /* Create a dtrace handle. */
    dtrace_handle = dtrace_open(DTRACE_VERSION, 0, &rtrn);
    if(dtrace_handle == NULL)
    {
        fprintf(stderr, "failed to initialize dtrace: %s\n",
                dtrace_errmsg(NULL, rtrn));
        return (-1);
    }

    /* Set dtrace options. */
    (void)dtrace_setopt(dtrace_handle, "bufsize", "4m");
    (void)dtrace_setopt(dtrace_handle, "aggsize", "4m");

    /* Compile the dtrace program. */
    prog = dtrace_program_strcompile(dtrace_handle, dtrace_prog,
                                     DTRACE_PROBESPEC_NAME, 0, 0, NULL);
    if(prog == NULL)
    {
        output(ERROR, "Failed to compile dtrace program\n");
        return (-1);
    }

    /* Send the dtrace program to the kernel and execute it. */
    rtrn = dtrace_program_exec(dtrace_handle, prog, &info);
    if(rtrn < 0)
    {
        output(ERROR, "Failed to enable dtrace probes\n");
        return (-1);
    }

    /* Inject and start the probes. */
    rtrn = dtrace_go(dtrace_handle);
    if(rtrn < 0)
    {
        output(ERROR, "Can't start probes\n");
        return (-1);
    }

    /* Sleep so we don't record any information. */
    dtrace_sleep(dtrace_handle);

    return (0);
}

int32_t setup_probe_module(char *exec_path)
{
    /* Set the path to the target executable. */
    target_path = exec_path;

    return (0);
}

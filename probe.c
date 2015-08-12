

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
#include "utils.h"
#include "nextgen.h"
#include <inttypes.h>
#include <capstone/capstone.h>
#include <dtrace.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

int inject_kernel_probes(void)
{
    
    return 0;
}

int inject_fork_server(void)
{
    return _inject_fork_server();
}

static char *create_dtrace_script(void)
{
    
    char *script = malloc(256);
    if(script == NULL)
    {
        output(ERROR, "Can't malloc dprog\n");
        return NULL;
    }

    /* Create dtrace script to instrument all instructon in the target process. */
    int rtrn = sprintf(script, "pid%d:::", map->exec_ctx->pid);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create dtrace script: %s\n", strerror(errno));
        return NULL;
    }

    return script;
}


/* To inject probes we need to get */
int inject_probes(void)
{
    output(STD, "Injecting probes in target binary\n");

    int fd, rtrn;
    char *file_buffer;
    off_t size;

    /* Lets create a dtrace handle. */
    dtrace_hdl_t *dtrace_handle;
    dtrace_handle = dtrace_open(DTRACE_VERSION, 0, &rtrn);
    if(dtrace_handle == NULL)
    {
        output(ERROR, "Can't create dtrace handle\n");
        return -1;
    }

    /* Get the end virtual memory address for the binary. */
    disas_executable_and_examine();

    /*
     * Set our buffer size and aggregation buffer size to the de facto
     * standard of 4M.
     */
    (void) dtrace_setopt(dtrace_handle, "bufsize", "4m");
    (void) dtrace_setopt(dtrace_handle, "aggsize", "4m");

    /* Lets create a dtrace script that inserts probes at every instruction. */
    char *dtrace_prog = create_dtrace_script();
    if(dtrace_prog == NULL)
    {
        output(ERROR, "Can't create dtrace script. \n");
        return -1;
    }

    dtrace_prog_t *prog;
    dtrace_proginfo_t info;

    /* Lets compile the dtrace script. */
    prog = dtrace_program_strcompile(dtrace_handle, dtrace_prog, DTRACE_PROBESPEC_NAME, 0, 0, NULL);
    if(prog == NULL)
    {
        output(ERROR, "Can't compile dtrace prog.\n");
        return -1;
    }

    /* Now we execute the dtrace scripts. */
    rtrn = dtrace_program_exec(dtrace_handle, prog, &info);
    if(rtrn < 0)
    {
        output(ERROR, "Can't execute dtrace script.\n");
        return -1;
    }

    return 0;
}



#include "probe.h"
#include "disas.h"
#include "utils.h"
#include "nextgen.h"
#include <inttypes.h>
#include <capstone/capstone.h>
#include <dtrace.h>
#include <errno.h>
#include <string.h>
#include <machine/reg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>

int inject_fork_server(void)
{
    output(STD, "Creating fork server\n");

    struct reg regs;

    map->exec_ctx->pid = fork();
    if(map->exec_ctx->pid == 0)
    {
        /* Let's announce we want to be traced so that we don't execute any instructions on execv. */
        ptrace(PT_TRACE_ME, 0, NULL, 0);

        /* Now we execute the target binary. */
        execv(map->path_to_exec, map->exec_ctx->args);
    }
    else if(map->exec_ctx->pid > 0)
    {
        int status;

        /* Let's wait until the target binary has stopped. */
        wait4(map->exec_ctx->pid, &status, 0, NULL);

        /* Lets check the reason why were not waiting anymore. Hopefully it's
         because the target executable is stopped. */
        

        /* Lets save the code at main in the target process. */
        int orig = ptrace(PT_READ_I, map->exec_ctx->pid, (caddr_t)&map->exec_ctx->main_start_address, 0);

        /* Let's set a breakpoint on main. */
        ptrace(PT_WRITE_I, map->exec_ctx->pid, (caddr_t)&map->exec_ctx->main_start_address, (orig & TRAP_MASK) | TRAP_INST);

        /* Lets continue until the breakpoint. */
        ptrace(PT_CONTINUE, map->exec_ctx->pid, (caddr_t)1, 0);

        waitpid(map->exec_ctx->pid, &status, 0);

        /* Lets grab the registers at main/breakpoint .*/
        ptrace(PT_GETREGS, map->exec_ctx->pid, (caddr_t)&regs, 0);

        printf("rax: %lu\n", regs.r_rax);

        /* Now we have to */

        return 0;
    }
    else
    {
        output(ERROR, "fork-server failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

/* We use this funcction to create and return a dtrace script that sticks a probe every 5th address. */
static char *createDtraceScript(uint64_t end_address_offset)
{
    char *script = NULL;

    int rtrn = asprintf(&script, "#!/usr/sbin/dtrace -s\n");
    if(rtrn < 0)
    {
        output(ERROR, "Can't create Dtrace script: %s\n", strerror(errno));
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

    /* Lets get the end  virtual memory address for the binary. */
    disas_executable();

    /*
     * Set our buffer size and aggregation buffer size to the de facto
     * standard of 4M.
     */
    (void) dtrace_setopt(dtrace_handle, "bufsize", "4m");
    (void) dtrace_setopt(dtrace_handle, "aggsize", "4m");

    /* Lets create a dtrace script that inserts probes every 0x5 address. */
    char *dtrace_prog = createDtraceScript(map->exec_ctx->end_offset);
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

    /* Now lets execute the dtrace scripts. */
    rtrn = dtrace_program_exec(dtrace_handle, prog, &info);
    if(rtrn < 0)
    {
        output(ERROR, "Can't execute dtrace script.\n");
        return -1;
    }

    return 0;
}
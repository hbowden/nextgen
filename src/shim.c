

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

#include "shim.h"
#include "io.h"
#include "probe.h"
#include "nextgen.h"

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef FREEBSD

#include "freebsd_syscall_table.h"

#include <gelf.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <machine/reg.h>

struct syscall_table *get_table(void)
{
    return freebsd_syscall_table;
}

int _inject_fork_server(void)
{
    output(STD, "Creating fork server\n");

    struct reg regs;

    int status;
        
    /* Lets save the code at main in the target process. */
    int orig = ptrace(PT_READ_I, map->exec_ctx->pid, (caddr_t)&map->exec_ctx->main_start_address, 0);

    /* Let's set a breakpoint on main. */
    ptrace(PT_WRITE_I, map->exec_ctx->pid, (caddr_t)&map->exec_ctx->main_start_address, (orig & TRAP_MASK) | TRAP_INST);

    /* Now we continue until the breakpoint. */
    ptrace(PT_CONTINUE, map->exec_ctx->pid, (caddr_t)1, 0);

    /* Wait until the breakpoint is hit. */
    wait4(map->exec_ctx->pid, &status, 0, NULL);

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
    ptrace(PT_GETREGS, map->exec_ctx->pid, (caddr_t)&regs, 0);

    printf("rax: Ox%lx\n", regs.r_rax);

    return 0;
}

int _get_load_address(void)
{
    GElf_Ehdr ehdr;
    Elf *elf = NULL;
    int fd, rtrn;

    /* Open the file. */
    fd = open(map->exec_ctx->path_to_exec, O_RDONLY);
    if(fd < 0)
    {
        output(ERROR, "open: %s\n", strerror(errno));
        return -1;
    }

    rtrn = elf_version(EV_CURRENT);
    if(rtrn == EV_NONE)
    {
        output(ERROR, "Can't init elf library: %s\n", elf_errmsg(elf_errno()));
        return -1;
    }

   /* Let's read the file. */
    elf = elf_begin(fd, ELF_C_READ, NULL);
    if(elf == NULL)
    {
        output(ERROR, "Can't read an elf file: %s\n", elf_errmsg(elf_errno()));
        return -1;
    }

    /* Let's make sure the file is a elf file. */
    switch (elf_kind(elf))
    {
        case ELF_K_NONE:
            output(ERROR, "Not a ELF file.\n");
            return -1;

        case ELF_K_ELF:
            if(gelf_getehdr(elf, &ehdr) == NULL)
            {
                output(ERROR, "Can't rget elf header: %s\n", elf_errmsg(elf_errno()));
                return -1;
            }

            map->exec_ctx->main_start_address = ehdr.e_entry;
            
            break;

        case ELF_K_AR:
            output(ERROR, "This is an archive not an executable.\n");
            return -1;

        default:
            output(ERROR, "Unknown file type.\n");
            return -1;
    }
    elf_end(elf);
    return 0;
}

#endif

/* End of FreeBSD definition. Begining of Mac OSX. */

#ifdef MAC_OSX

#include "mac_osx_syscall_table.h"

struct syscall_table *get_table(void)
{
    return mac_osx_syscall_table;
}

int _inject_fork_server(void)
{
    output(STD, "Creating fork server\n");

    return 0;
}

int _get_load_address(void)
{
    return 0;
}

#endif



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

#include "shim.h"
#include "io/io.h"
#include "probe.h"
#include "file.h"
#include "concurrent.h"
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
    /* Return the FreeBSD system call table object. */
    return (freebsd_syscall_table);
}

int32_t _get_load_address(void)
{
    int32_t fd = 0;
    int32_t rtrn = 0;
    GElf_Ehdr ehdr;
    Elf *elf = NULL;
    char *path_to_exec = NULL;

    /* Grab the executable path. */
    rtrn = get_exec_path(&path_to_exec);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get target exec path\n");
        return (-1);
    }

    /* Open the file. */
    fd = open(path_to_exec, O_RDONLY);
    if(fd < 0)
    {
        output(ERROR, "open: %s\n", strerror(errno));
        return (-1);
    }

    rtrn = elf_version(EV_CURRENT);
    if(rtrn == EV_NONE)
    {
        output(ERROR, "Can't init elf library: %s\n", elf_errmsg(elf_errno()));
        return (-1);
    }

   /* Let's read the file. */
    elf = elf_begin(fd, ELF_C_READ, NULL);
    if(elf == NULL)
    {
        output(ERROR, "Can't read an elf file: %s\n", elf_errmsg(elf_errno()));
        return (-1);
    }

    /* Let's make sure the file is a elf file. */
    switch (elf_kind(elf))
    {
        case ELF_K_NONE:
            output(ERROR, "Not a ELF file.\n");
            return (-1);

        case ELF_K_ELF:
            if(gelf_getehdr(elf, &ehdr) == NULL)
            {
                output(ERROR, "Can't rget elf header: %s\n", elf_errmsg(elf_errno()));
                return (-1);
            }

            set_start_addr(ehdr.e_entry);
            
            break;

        case ELF_K_AR:
            output(ERROR, "This is an archive not an executable.\n");
            return -1;

        case ELF_K_COFF:
        case ELF_K_NUM:
            output(ERROR, "Can't handle this file type.\n");
            return -1;

        /* Don't need a default case because we handle all the possible enumerations. */
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

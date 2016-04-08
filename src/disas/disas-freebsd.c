/**
 * Copyright (c) 2016, Harrison Bowden, Minneapolis, MN
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

#include "disas-freebsd.h"
#include "io/io.h"

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <gelf.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <machine/reg.h>

int32_t get_load_address(uint64_t *main_addr)
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

	return (0);
}

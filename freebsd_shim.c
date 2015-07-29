

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
#include "nextgen.h"

struct syscall_table *get_table(void)
{
	return freebsd_syscall_table;
}

int _get_load_address(void)
{
	GElf_Ehdr ehdr;
    Elf *elf = NULL;
    int fd, rtrn;

    /* Open the file. */
    fd = open(map->path_to_exec, O_RDONLY);
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
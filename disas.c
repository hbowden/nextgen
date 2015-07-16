
#include "disas.h"
#include "utils.h"
#include "nextgen.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <inttypes.h>
#include <capstone/capstone.h>

#ifdef FREEBSD

#include <gelf.h>

#endif

#ifdef MAC_OSX



#endif

#ifdef MAC_OSX
/* This function looks up the address that main starts at
in the target executable. On Mac OSX 10.8 and beyond this means
looking at the mach-o header in the executable, and looking
for LC_MAIN. The address at LC_MAIN is the address of _main
in the target executable. */
int get_load_address(char *path,  uint64_t *start_address)
{
   

    return 0;
}

#endif

#ifdef FREEBSD

int get_load_address(void)
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
#endif

int disas_executable(void)
{
    int fd, rtrn;
    char *file_buffer;
    off_t size;

    fd = open(map->path_to_exec, O_RDONLY);
    if(fd < 0)
    {
    	output(ERROR, "open: %s\n", strerror(errno));
    	return -1;
    }

    rtrn = map_file_in(fd, &file_buffer, &size);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't memory map file\n");
    	return -1;
    }

    csh handle;
    cs_open(CS_ARCH_X86, CS_MODE_32, &handle);
 
    // allocate memory cache for 1 instruction, to be used by cs_disasm_iter later.
    cs_insn *insn = cs_malloc(handle);

    uint64_t address = 0;

    // disassemble one instruction a time & store the result into @insn variable above
    while(cs_disasm_iter(handle, &file_buffer, (unsigned long *)&size, &address, insn))
    {
        /* Check disasembly so we can figure out the various branches. */
    }

    /* Save the offset of where the program ends. */
    map->exec_ctx->end_offset = insn[0].address;

    // release the cache memory when done
    cs_free(insn, 1);

    return 0;
}
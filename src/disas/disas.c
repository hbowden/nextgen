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

#include "disas.h"
#include "utils/utils.h"
#include "file/file.h"
#include "memory/memory.h"
#include "io/io.h"
#include "capstone.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <inttypes.h>

int32_t disas_executable_and_examine(void)
{
    int32_t rtrn;
    uint64_t file_size;
    uint32_t count = 0;
    int32_t file auto_close = 0;
    char *file_buffer = NULL;
    char *exec_path auto_free = NULL;

    /* Ask the file module for the path of the binary to test. */
    rtrn = get_exec_path(&exec_path);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get exec path\n");
        return -1;
    }

    /* Open the target binary. */
    file = open(exec_path, O_RDONLY);
    if(file < 0)
    {
    	output(ERROR, "open: %s\n", strerror(errno));
    	return (-1);
    }

    /* Read file in to memory. */
    rtrn = map_file_in(file, &file_buffer, &file_size, READ);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't memory map file\n");
    	return (-1);
    }

    /* Create a const pointer to the file buffer so we can avoid
    a warning when using cs_disasm_iter(). */
    const uint8_t *file_buffer_copy = (uint8_t *)file_buffer;
    
    /* Create capstone handle.*/
    csh handle;
    cs_open(CS_ARCH_X86, CS_MODE_32, &handle);
 
    /* Allocate memory cache for 1 instruction, to be used by cs_disasm_iter later. */
    cs_insn *insn = cs_malloc(handle);

    uint64_t address = 0;

    /* Disassemble one instruction at a time & store the result into the insn variable above */
    while(cs_disasm_iter(handle, (const uint8_t **)&file_buffer_copy, (unsigned long *)&file_size, &address, insn))
    {
        /* Check for branchs in the disasembly, maybe switch to a hash map in the future. */
        if(strncmp(insn->mnemonic, "jne", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "je", 2) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jg", 2) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jle", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jl", 2) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jge", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jo", 2) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jno", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "js", 2) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jns", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jz", 2) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jb", 2) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jnae", 4) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jc", 2) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jnb", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jae", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jnc", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jbe", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jna", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "ja", 2) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jnbe", 4) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jl", 2) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jnge", 4) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jge", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jng", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jnle", 4) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jp", 2) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jpe", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jnp", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jpo", 3) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jcxz", 4) == 0)
        {
            goto found_branch;
        }
        else if(strncmp(insn->mnemonic, "jecxz", 5) == 0)
        {
            goto found_branch;
        }
        else
        {
            /* We didn't find a branch instruction so cotinue looping */
            continue;
        }

    found_branch:

        /*map->exec_ctx->address_index = realloc(map->exec_ctx->address_index, sizeof(char *) * count);
        if(map->exec_ctx->address_index == NULL)
        {
            output(ERROR, "Can't realloc address index: %s\n", strerror(errno));
            return -1;
        }

        rtrn = asprintf(&map->exec_ctx->address_index[count], "%s", insn->address);
        if(rtrn < 0)
        {
            output(ERROR, "Can't alloc address index slot: %s\n", strerror(errno));
            return -1;
        } */

        count++;
        
        continue;
    }

    /* Save the offset of where the program ends. */
    rtrn = set_end_offset(insn[0].address);
    if(rtrn < 0)
    {
        output(ERROR, "Can't set end offset\n");
        return -1;
    }

    cs_free(insn, 1); // release the cache memory when done
    munmap(file_buffer, (size_t)file_size);

    return 0;
}

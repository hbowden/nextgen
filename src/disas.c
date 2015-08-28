

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

#include "disas.h"
#include "utils.h"
#include "shim.h"
#include "nextgen.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <capstone/capstone.h>

int get_load_address(void)
{
    /* Call the platform specific get_load_address() function.  */
    _get_load_address();

    return 0;
}

int disas_executable_and_examine(void)
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
    while(cs_disasm_iter(handle, (const uint8_t **)&file_buffer, (unsigned long *)&size, &address, insn))
    {
        /* Check disasembly so we can figure out the various branches. */
    }

    /* Save the offset of where the program ends. */
    map->exec_ctx->end_offset = insn[0].address;

    // release the cache memory when done
    cs_free(insn, 1);

    return 0;
}
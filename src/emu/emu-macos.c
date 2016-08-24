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

#include "emu.h"
#include "io/io.h"
#include "memory/memory.h"
#include <stdint.h>
#include <mach-o/loader.h>
#include <mach-o/swap.h>

struct program_header
{
    uint32_t      magic;
    cpu_type_t    cputype;
    cpu_subtype_t cpusubtype;
    uint32_t      filetype;
    uint32_t      ncmds;
    uint32_t      sizeofcmds;
    uint32_t      flags;
};

static uint32_t read_magic(FILE *fp)
{
    uint32_t magic;
    fseek(fp, 0, SEEK_SET);
    fread(&magic, sizeof(uint32_t), 1, fp);
    return (magic);
}

int32_t get_magic_number(FILE *fp, uint32_t *magic)
{
    (*magic) = read_magic(fp);
    return (0);
}

struct program_header *init_header(void)
{
    struct program_header *header = NULL;

    header = mem_calloc(sizeof(struct program_header));
    if(header == NULL)
    {
        output(ERROR, "Program header allocation failed\n");
        return (NULL);
    }

    return (header);
}

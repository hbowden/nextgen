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
#include "io/io.h" // Needed for output().
#include "utils/utils.h"  // Used for auto_close attribute.
#include "memory/memory.h" // Needed for mem_calloc(), and mem_free().
#include "concurrent/epoch.h" // Needed for epoch symbols.

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>

/* An opaque object the holds an executables header information. */
struct program_header;

/* */
struct program_ctx
{
    /* Contains information extracted from
    the executable's header. */
    struct prog_header *header;

};

struct emulator_ctx
{
    /* This object tracks information related to the program being
       emulated. This object is replaced on each call to load_file(). */
    struct program_ctx *prog;

    /* Epoch object, tracks emulator thread states,
    to implement a epoch based memory reclamation scheme. */
    epoch_ctx epoch;
};

struct emulator_ctx *init_emulator(void)
{
    struct emulator_ctx *emu = NULL;

    /* Allocate and initialize emulator object. */
    emu = mem_calloc(sizeof(struct emulator_ctx));
    if(emu == NULL)
    {
        output(ERROR, "Emulator allocation failed\n");
        return (0);
    }

    epoch_init(&emu->epoch);

    return (emu);
 }

void free_emulator(struct emulator_ctx **emu)
{
    mem_free((void **)emu);
    return;
}

static uint32_t read_magic(FILE *fp)
{
    uint32_t magic;
    fseek(fp, 0, SEEK_SET);
    fread(&magic, sizeof(uint32_t), 1, fp);
    return (magic);
}

static struct program_ctx *init_program_ctx(void)
{
    struct program_ctx *prog = NULL;

    prog = mem_calloc(sizeof(struct program_ctx));
    if(prog == NULL)
    {
        output(ERROR, "Program context allocation failed\n");
        return (NULL);
    }

    prog->header = mem_calloc(sizeof(struct prog_header));
    if(prog->header == NULL)
    {
        output(ERROR, "Program header allocation failed\n");
        return (NULL);
    }

    return (0);
}

static int32_t parse_macho_fp(struct emulator_ctx *emu, FILE *fp)
{
    /* Create a new program context object. */
    emu->prog = init_program_ctx();
    if(emu->prog == NULL)
    {
        output(ERROR, "Program context initialization failed");
        return (-1);
    }

    /* Grab the magic number so we can determine  */
    emu->prog->header->magic = read_magic(fp);

    return (0);
}

int32_t load_file(struct emulator_ctx *emu, const char *path)
{
    if(path == NULL)
    {
        output(ERROR, "path is NULL\n");
        return (-1);
    }

    FILE *fp auto_close_fp = NULL;
    fp = fopen(path, "r");
    if(fp == NULL)
    {
        output(ERROR, "Failed to open file: %s\n", strerror(errno));
        return (-1);
    }

    parse_macho_fp(fp);

    struct stat stbuf;
    if(fstat(fd, &stbuf) != 0)
    {
        output(ERROR, "fstat failed: %s\n", strerror(errno));
        return (-1);
    }

    emu->program_data = mmap(NULL, stbuf.st_size, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, 0);
    if(emu->program_data == MAP_FAILED)
    {
        output(ERROR, "mmap: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

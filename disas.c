
#include "disas.h"
#include "/users/nahnig/desktop/distorm/include/distorm.h"
#include "utils.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#ifdef FREEBSD

#include <libelf.h>

#endif

#ifdef MAC_OSX

#include "libmacho/macho.h"

#endif

// The number of the array of instructions the decoder function will use to return the disassembled instructions.
// Play with this value for performance...
#define MAX_INSTRUCTIONS 1000

/* This function looks up the address that main starts at
in the target executable. On Mac OSX 10.8 and beyond this means
looking at the mach-o header in the executable, and looking
for LC_MAIN. The address at LC_MAIN is the address of _main
in the target executable. */
int get_load_address(char *path,  uint32_t *start_address)
{
    macho_t *macho = macho_open(path);
    if(macho == NULL)
    {
        output(ERROR, "Unable to open macho file\n");
        return -1;
    }

    macho_command_t *command = macho_command_load(macho->data, MACHO_CMD_UNIXTHREAD);
    *start_address = command->offset;

    return 0;
}

int disas_executable(char *path)
{
    int fd, rtrn;
    char *file_buffer;
    off_t size;
    int offset = 0;

    fd = open(path, O_RDONLY);
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

    // Holds the result of the decoding.
    _DecodeResult res;

    // Decoded instruction information.
    _DecodedInst decodedInstructions[MAX_INSTRUCTIONS];

    // next is used for instruction's offset synchronization.
    // decodedInstructionsCount holds the count of filled instructions' array by the decoder.
    unsigned int decodedInstructionsCount = 0, next;
    //int i;
    _DecodeType dt = Decode64Bits;

    // Decode the buffer at given offset (virtual address).
    while (1) 
    {
        // If you get an undefined reference linker error for the following line,
        // change the SUPPORT_64BIT_OFFSET in distorm.h.
        res = distorm_decode(offset, (const unsigned char*)file_buffer, (int)size, dt, decodedInstructions, MAX_INSTRUCTIONS, &decodedInstructionsCount);
        if (res == DECRES_INPUTERR) {

            output(ERROR, "Input error, halting!\n", stderr);
            return -1;
        }

        //for (i = 0; i < decodedInstructionsCount; i++)
            //printf("%0*lx (%02d) %-24s %s%s%s\r\n", dt != Decode64Bits ? 8 : 16, decodedInstructions[i].offset, decodedInstructions[i].size, (char*)decodedInstructions[i].instructionHex.p, (char*)decodedInstructions[i].mnemonic.p, decodedInstructions[i].operands.length != 0 ? " " : "", (char*)decodedInstructions[i].operands.p);


        if (res == DECRES_SUCCESS) break; // All instructions were decoded.
        else if (decodedInstructionsCount == 0) break;

        // Synchronize:
        next = (unsigned int)(decodedInstructions[decodedInstructionsCount-1].offset - offset);
        next += decodedInstructions[decodedInstructionsCount-1].size;

        // Advance ptr and recalc offset.
        file_buffer += next;
        offset += next;
    }
    return 0;
}
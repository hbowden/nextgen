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

#ifndef EMU_H
#define EMU_H

#include <stdint.h>

struct emulator_ctx;

/**
 * Allocates and returns a new emulator context object. This object
 * represents one instance of a emulator. The emulator object
 * should be freed with free_emulator(), when you are done with
 * it.
 */
extern struct emulator_ctx *init_emulator(void);

/**
 * Frees resources tied to the emulator object.
 *
 * @param emu A pointer to a emulator object pointer to be freed.
 */
extern void free_emulator(struct emulator_ctx **);


/**
 * Loads the file at the path passed into memory.
 *
 *
 */
extern int32_t load_file(struct emulator_ctx *, const char *);

#endif

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

#include "unity.h"

/* Include the C source file so we can test opaque structs. */
#include "../../../src/emu/emu.c"

static void test_load_file(void)
{
   struct emulator_ctx *emu = NULL;

   emu = init_emulator();
   TEST_ASSERT_NOT_NULL(emu);

   /* Load_file() should fail when given a bad file path. */
   int32_t rtrn = load_file(emu, "bogus/file/path");
   TEST_ASSERT(rtrn == -1);

   /* Load_file() should fail when given a NULL path. */
   rtrn = load_file(emu, NULL);
   TEST_ASSERT(rtrn == -1);

   /* Load_file() should return zero when given a valid executable. */
   rtrn = load_file(emu, "../tests/emu/helper_files/test");
   TEST_ASSERT(rtrn == 0);
   TEST_ASSERT_NOT_NULL(emu->program_data);

   free_emulator(&emu);

   /* Should be set to NULL after calling free_emulator(). */
   TEST_ASSERT_NULL(emu);

   return;
}

int main(void)
{
    test_load_file();

    return (0);
}

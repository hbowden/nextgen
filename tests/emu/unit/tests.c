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
#include "../../../src/emu/emu-macos.c"

static void test_free_emulator(void)
{
    struct emulator_ctx *emu = NULL;

    emu = init_emulator();
    TEST_ASSERT_NOT_NULL(emu);

    free_emulator(&emu);

    /* Should be set to NULL after calling free_emulator(). */
    TEST_ASSERT_NULL(emu);

    return;
}

static void test_init_emulator(void)
{
    struct emulator_ctx *emu = NULL;

    emu = init_emulator();
    TEST_ASSERT_NOT_NULL(emu);
    TEST_ASSERT_NOT_NULL(&emu->epoch);

    return;
}

int main(void)
{
    test_init_emulator();
    test_free_emulator();

    return (0);
}

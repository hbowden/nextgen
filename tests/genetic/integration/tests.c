/*
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
 */

#include "unity.h"
#include "syscall/syscall.h"
#include "genetic/genetic.c"
#include "concurrent/concurrent.h"

static void test_init_world(void)
{
    int32_t rtrn = 0;

    /* Initialize all the data structures. */
    rtrn = init_world();   

    /* init_world() should return on zero. */ 
    TEST_ASSERT(rtrn == 0);

    /* World should not be NULL. */
    TEST_ASSERT_NOT_NULL(world);
    TEST_ASSERT(world->total_species > 0);
    TEST_ASSERT(world->current_generation == 0);
    TEST_ASSERT_NOT_NULL(world->species);

    uint32_t i;

    for(i = 0; i < world->total_species - 1; i++)
    {
    	TEST_ASSERT_NOT_NULL(world->species[i]);
    	TEST_ASSERT(world->species[i]->species_population == 0);

    	struct organism_ctx *organism = NULL;

    	CK_SLIST_FOREACH(organism, &world->species[i]->organism_list, list_entry)
    	{
    		TEST_ASSERT_NOT_NULL(organism);
    		TEST_ASSERT_NOT_NULL(organism->chromosome);
    	}
    }
    return;
}

int main(void)
{
    int32_t rtrn = 0;
    int32_t stop_flag = FALSE;
    uint32_t counter = 0;
    epoch_ctx epoch;

    /* Initialize the syscall module before using the genetic module. */
    rtrn = setup_syscall_module(&stop_flag, &counter, TRUE, &epoch);
    TEST_ASSERT(rtrn == 0);

    test_init_world();

    return (0);
   
}
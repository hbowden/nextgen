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
#include "concurrent/thread.h"

void test_get_record(void)
{
	/* We need an initialized epoch context object
	  to initialize a thread context object. 
	epoch_ctx epoch;
	epoch_init(&epoch);
	TEST_ASSERT_NOT_NULL(&epoch);

     Initialize a thread context object and make sure it's not NULL.
	struct thread_ctx *thread = NULL;
	thread = init_thread(&epoch);
	TEST_ASSERT_NOT_NULL(thread);

	 Make sure the epoch record returned is not NULL.
	epoch_record *record = get_record(thread);
	TEST_ASSERT_NOT_NULL(record);

	*/

	return;
}

void test_thread_init(void)
{
	/* We need an initialized epoch context object
	  to initialize a thread context object. */
	epoch_ctx epoch;
	epoch_init(&epoch);
	TEST_ASSERT_NOT_NULL(&epoch);

    /* Initialize a thread context object and make sure it's not NULL. */
	struct thread_ctx *thread = NULL;
	thread = init_thread(&epoch);
	TEST_ASSERT_NOT_NULL(thread);

	return;
}

int main(void)
{
	test_thread_init();
	test_get_record();
	
    return (0);
}
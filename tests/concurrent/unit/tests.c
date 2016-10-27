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
#include "io/io.h"
#include "memory/memory.h"
#include "concurrent/epoch.h"
#include "concurrent/concurrent.h"
#include <pthread.h>

struct test_obj
{
   uint32_t counter;
};

static struct test_obj *obj;

static void *start_thread(void *arg)
{
	/* We need an initialized epoch context object
	  to initialize a thread context object. */
	epoch_ctx *e = (epoch_ctx *)arg;
	epoch_ctx epoch = (*e);

  /* We will also need a memory allocator for creating a thread object.  */
	struct memory_allocator *allocator = NULL;
	allocator = get_default_allocator();
	TEST_ASSERT_NOT_NULL(allocator);

  /* Create a output interface so init_thread() can log errors. */
  struct output_writter *output = NULL;
	output = get_console_writter();
	TEST_ASSERT_NOT_NULL(output);

  /* Initialize a thread context object and make sure it's not NULL. */
	struct thread_ctx *thread = NULL;
	thread = init_thread(&epoch, allocator, output);
	TEST_ASSERT_NOT_NULL(thread);

	/* Make sure the epoch record returned is not NULL. */
	epoch_record *record = get_record(thread);
	TEST_ASSERT_NOT_NULL(record);

	uint32_t i;

	for(i = 0; i < 100; i++)
	{
	    /* Start epoch protected section. */
        int32_t rtrn = epoch_start(thread, allocator, output);
        TEST_ASSERT(rtrn != -1);

        struct test_obj *o = atomic_load_ptr(&obj);
        atomic_add_uint32(&o->counter, 1);

        /* End the epoch protected section. */
        epoch_stop(thread, allocator);
	}

	  return (NULL);
}

static void test_epoch_section(void)
{
	  struct memory_allocator *allocator = NULL;
	  allocator = get_default_allocator();
	  TEST_ASSERT_NOT_NULL(allocator);

	  obj = allocator->shared(sizeof(struct test_obj));
	  TEST_ASSERT_NOT_NULL(obj);

	  /* Create a output interface so we can log errors. */
	  struct output_writter *output = NULL;
	  output = get_console_writter();
	  TEST_ASSERT_NOT_NULL(output);

	  obj->counter = 0;

    /* We need an initialized epoch context object
	  to initialize a thread context object. */
	  epoch_ctx epoch;
	  epoch_init(&epoch);
	  TEST_ASSERT_NOT_NULL(&epoch);

    int32_t rtrn = 0;
	  pthread_t pthread = 0;
	  rtrn = pthread_create(&pthread, NULL, start_thread, &epoch);
	  TEST_ASSERT(rtrn > -1);

    /* Initialize a thread context object and make sure it's not NULL. */
	  struct thread_ctx *thread = NULL;
	  thread = init_thread(&epoch, allocator, output);
	  TEST_ASSERT_NOT_NULL(thread);

	  /* Make sure the epoch record returned is not NULL. */
	  epoch_record *record = get_record(thread);
	  TEST_ASSERT_NOT_NULL(record);

	  uint32_t i;

	  for(i = 0; i < 100; i++)
	  {
	      /* Start epoch protected section. */
        rtrn = epoch_start(thread, allocator, output);
        TEST_ASSERT(rtrn != -1);

        struct test_obj *o = atomic_load_ptr(&obj);
        atomic_add_uint32(&o->counter, 1);

        /* End the epoch protected section. */
        epoch_stop(thread, allocator);
	  }

	  pthread_join(pthread, NULL);

	  TEST_ASSERT(obj->counter == 200);

	  return;
}

static void test_thread_init(void)
{
	  /* We need an initialized epoch context object
	    to initialize a thread context object. */
	  epoch_ctx epoch;
	  epoch_init(&epoch);
	  TEST_ASSERT_NOT_NULL(&epoch);

		/* We will also need a memory allocator for creating a thread object.  */
		struct memory_allocator *allocator = NULL;
		allocator = get_default_allocator();
		TEST_ASSERT_NOT_NULL(allocator);

	  /* Create a output interface so init_thread() can log errors. */
	  struct output_writter *output = NULL;
		output = get_console_writter();
		TEST_ASSERT_NOT_NULL(output);

    /* Initialize a thread context object and make sure it's not NULL. */
	  struct thread_ctx *thread = NULL;
	  thread = init_thread(&epoch, allocator, output);
	  TEST_ASSERT_NOT_NULL(thread);

	  return;
}

int main(void)
{
	  test_thread_init();
	  test_epoch_section();

    return (0);
}

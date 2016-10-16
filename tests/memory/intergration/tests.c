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
#include "memory/memory.h"
#include <pthread.h>

static uint32_t count = 1024;
static uint32_t iterations = 100000;

struct test_obj
{
	uint32_t var;
	uint32_t var2;
};

static void *thread_start(void *arg)
{
    uint32_t i;
    struct mem_pool_shared *pool = (struct mem_pool_shared *)arg;
    struct memory_block *m_blk = NULL;

    /* Retrieve all the blocks, then return them. */
    for(i = 0; i < iterations; i++)
    {
        m_blk = mem_get_shared_block(pool);

        TEST_ASSERT_NOT_NULL(m_blk);
        TEST_ASSERT_NOT_NULL(m_blk->ptr);

        mem_free_shared_block(m_blk, pool);
    }

    return (NULL);
}

static void test_shared_pool(void)
{
	/* Declare a memory pool. */
    struct mem_pool_shared *pool = NULL;

    /* Pool should be NULL if the block size is zero. */
    pool = mem_create_shared_pool(0, count);
    TEST_ASSERT_NULL(pool);

    /* Pool should be NULL if the block count is zero. */
    pool = mem_create_shared_pool(sizeof(struct test_obj), 0);
    TEST_ASSERT_NULL(pool);

    /* Create a memory pool, pass the size of the object to store as the first argument.
       and the number of objects you want to store as the second argument. */
    pool = mem_create_shared_pool(sizeof(struct test_obj), count);
    TEST_ASSERT_NOT_NULL(pool);

    TEST_ASSERT(pool->block_size == sizeof(struct test_obj));
    TEST_ASSERT(pool->block_count == count);

    uint32_t i = 0;
    struct memory_block *m_blk = NULL;
    uint32_t number_of_blocks = 0;

    /* Initialize and insert the objects we want to store into the pool. */
    init_shared_pool(&pool, m_blk)
    {
        TEST_ASSERT_NOT_NULL(pool);
        TEST_ASSERT_NOT_NULL(m_blk);
        TEST_ASSERT_NOT_NULL(m_blk->ptr);

        struct test_obj *obj = NULL;
        obj = mem_alloc_shared(sizeof(struct test_obj));

        TEST_ASSERT_NOT_NULL(obj);

        m_blk->ptr = obj;

        number_of_blocks++;
    }

    /* Make sure the number of blocks we put in is how much that came out. */
    TEST_ASSERT(number_of_blocks == count);

    /* Retrieve all the blocks, then return them. */
    for(i = 0; i < iterations; i++)
    {
        m_blk = mem_get_shared_block(pool);

        TEST_ASSERT_NOT_NULL(m_blk);
        TEST_ASSERT_NOT_NULL(m_blk->ptr);

        mem_free_shared_block(m_blk, pool);
    }

    int32_t rtrn = 0;
    pthread_t thread1, thread2, thread3;

    rtrn = pthread_create(&thread1, NULL, thread_start, pool);
    TEST_ASSERT(rtrn == 0)

    rtrn = pthread_create(&thread2, NULL, thread_start, pool);
    TEST_ASSERT(rtrn == 0)

    rtrn = pthread_create(&thread3, NULL, thread_start, pool);
    TEST_ASSERT(rtrn == 0)

    /* Retrieve some blocks, then return them. */
    for(i = 0; i < iterations; i++)
    {
        m_blk = mem_get_shared_block(pool);

        TEST_ASSERT_NOT_NULL(m_blk);
        TEST_ASSERT_NOT_NULL(m_blk->ptr);

        mem_free_shared_block(m_blk, pool);
    }

    void *buf = NULL;

    rtrn = pthread_join(thread1, &buf);
    TEST_ASSERT(rtrn == 0)

    rtrn = pthread_join(thread2, &buf);
    TEST_ASSERT(rtrn == 0)

    rtrn = pthread_join(thread3, &buf);
    TEST_ASSERT(rtrn == 0)

	return;
}

int main()
{
    test_shared_pool();

	  return (0);
}

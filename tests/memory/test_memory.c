

/**
 * Copyright (c) 2015, Harrison Bowden, Secure Labs, Minneapolis, MN
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

#include "test_utils.h"
#include "memory.h"

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>

#define count 32768

struct test_obj
{
    int value;
    char *ptr;
};

static int test_mem_alloc(void)
{
    log_test(DECLARE, "Testing heap memory allocator");

    char *buf = NULL;

    /* Make sure mem_alloc() fails when passed a zero size. */
    buf = mem_alloc(0);
    assert_stat(buf == NULL);

    buf = mem_alloc(10);
    assert_stat(buf != NULL);

    mem_free(buf);

    log_test(SUCCESS, "Passed heap alloc test");

	return 0;
}

static int test_mem_alloc_shared(void)
{
	log_test(DECLARE, "Testing shared memory allocator");

    char *buf = NULL;

    /* Make sure mem_alloc_shared() fails when passed a zero size. */
    buf = mem_alloc_shared(0);
    assert_stat(buf == NULL);

    buf = mem_alloc_shared(10);
    assert_stat(buf != NULL);

    buf = "test";

    pid_t pid;
    int rtrn;
    int port[2];

    rtrn = pipe(port);
    if(rtrn < 0)
    {
        return -1;
    }

    pid = fork();
    if(pid == 0)
    {
        assert_stat(strncmp(buf, "test", 4) == 0);

        write(port[1], "1", 1);

        _exit(0);
    }
    else if(pid > 0)
    {
    	char *buf2 = mem_alloc(2);
    	if(buf2 == NULL)
    	{
    		return -1;
    	}

    	/* Wait for the child to be done setting up. */
        ssize_t ret = read(port[0], buf2, 1);
        if(ret < 1)
        {
            mem_free(buf2);
            return -1;
        }

        mem_free(buf2);
    }
    else
    {
    	return -1;
    }

	munmap(buf, 10);

	log_test(SUCCESS, "Passed shared alloc test");

    return 0;
}

static int test_shared_pool(void)
{
    log_test(DECLARE, "Testing shared memory pool");

    /* Declare a memory pool. */
    struct mem_pool_shared *pool = NULL;

    /* Pool should be NULL if the block size is zero. */
    pool = mem_create_shared_pool(0, count);
    assert_stat(pool == NULL);

    /* Pool should be NULL if the block count is zero. */
    pool = mem_create_shared_pool(sizeof(struct test_obj), 0);
    assert_stat(pool == NULL);

    /* Create a memory pool, pass the size of the object to store as the first argument.
       and the number of objects you want to store as the second argument. */
    pool = mem_create_shared_pool(sizeof(struct test_obj), count);
    
    /* Make sure pool is not NULL. */
    assert_stat(pool != NULL);

    assert_stat(pool->block_size == sizeof(struct test_obj));
    assert_stat(pool->block_count == 32768);

    uint32_t i = 0;
    struct memory_block *m_blk = NULL;
    uint32_t number_of_blocks = 0;

    /* Initialize and insert the objects we want to store into the pool. */
    init_shared_pool(&pool, m_blk)
    {
        assert_stat(pool != NULL);
        assert_stat(m_blk != NULL);
        assert_stat(m_blk->ptr != NULL);

        struct test_obj *obj = NULL;

        obj = mem_alloc_shared(sizeof(struct test_obj));

        assert_stat(obj != NULL);

        m_blk->ptr = obj;

        number_of_blocks++;
    }
    
    /* Make sure the number of blocks we put in is how much that came out. */
    assert_stat(number_of_blocks == 32768);

    /* Retrieve all the blocks, then return them. */
    for(i = 0; i < 32768; i++)
    {
        m_blk = mem_get_shared_block(pool);

        assert_stat(m_blk != NULL);
        assert_stat(m_blk->ptr != NULL);

        mem_free_shared_block(m_blk, pool);
    }

    log_test(SUCCESS, "Shared memory pool");

    return 0;
}

static int test_mem_calloc(void)
{
    log_test(DECLARE, "Testing mem calloc");



    

    log_test(SUCCESS, "Mem calloc test passed");

    return 0;
}

static int test_mem_calloc_shared(void)
{
    log_test(DECLARE, "Testing mem calloc shared");



    

    log_test(SUCCESS, "Mem calloc shared test passed");

    return 0;
}

int main(void)
{
    int rtrn = 0;

    /* Map the stats object as shared anonymous memory. */
    stat = init_stats_obj();
    if(stat == NULL)
    {
        output(ERROR, "Can't init the stats object\n");
        return -1;
    }

    rtrn = test_shared_pool();
    if(rtrn < 0)
        return -1;

    rtrn = test_mem_alloc();
    if(rtrn < 0)
        return -1;

    rtrn = test_mem_alloc_shared();
    if(rtrn < 0)
        return -1;

    rtrn = test_mem_calloc();
    if(rtrn < 0)
        return -1;

    rtrn = test_mem_calloc_shared();
    if(rtrn < 0)
        return -1;

    /* Output results. */
    output(STD, "[%d] %ld assertions passed, %ld test passed, and %ld test failed.\n", \
          (100 * stat->successes) / stat->test_ran, stat->asserts_ran, stat->successes, stat->fails);

    return 0;
}


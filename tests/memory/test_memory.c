/**
 * Copyright (c) 2015, Harrison Bowden, Minneapolis, MN
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
#include "memory/memory.h"

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>

#define count 32768

struct test_obj
{
    int32_t value;
    char *ptr;
};

static int32_t test_mem_free_shared(void)
{
    log_test(DECLARE, "Testing memory free shared");

    char *buffer = NULL;

    buffer = mem_alloc_shared(100);
    assert_stat(buffer != NULL);

    mem_free_shared((void **)&buffer, 100);
    assert_stat(buffer == NULL);

    log_test(SUCCESS, "Memory free shared test passed");

    return (0);
} 

static int32_t test_mem_free(void)
{
    log_test(DECLARE, "Testing memory free");

    char *buffer = NULL;

    buffer = mem_alloc(100);
    assert_stat(buffer != NULL);

    mem_free((void **)&buffer);
    assert_stat(buffer == NULL);

    log_test(SUCCESS, "Memory free test passed");

    return (0);
}

static int32_t test_mem_alloc(void)
{
    log_test(DECLARE, "Testing heap memory allocator");

    char *buf = NULL;

    /* Make sure mem_alloc() fails when passed a zero size. */
    buf = mem_alloc(0);
    assert_stat(buf == NULL);

    buf = mem_alloc(10);
    assert_stat(buf != NULL);

    mem_free((void **)&buf);

    log_test(SUCCESS, "Passed heap alloc test");

	return 0;
}

static int32_t test_mem_alloc_shared(void)
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
            mem_free((void **)&buf2);
            return -1;
        }

        mem_free((void **)&buf2);
    }
    else
    {
    	return -1;
    }

	munmap(buf, 10);

	log_test(SUCCESS, "Passed shared alloc test");

    return 0;
}

static void *thread_start(void *arg)
{
    uint32_t i;
    struct mem_pool_shared *pool = (struct mem_pool_shared *)arg;
    struct memory_block *m_blk = NULL;

    /* Retrieve all the blocks, then return them. */
    for(i = 0; i < count; i++)
    {
        m_blk = mem_get_shared_block(pool);

        assert_stat(m_blk != NULL);
        assert_stat(m_blk->ptr != NULL);

        mem_free_shared_block(m_blk, pool);
    }

    return (NULL);
}

static int32_t test_shared_pool(void)
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
    assert_stat(pool->block_count == count);

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
    assert_stat(number_of_blocks == count);

    /* Retrieve all the blocks, then return them. */
    for(i = 0; i < count; i++)
    {
        m_blk = mem_get_shared_block(pool);

        assert_stat(m_blk != NULL);
        assert_stat(m_blk->ptr != NULL);

        mem_free_shared_block(m_blk, pool);
    }

    int32_t rtrn = 0;
    pthread_t thread1, thread2;

    rtrn = pthread_create(&thread1, NULL, thread_start, pool);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create thread\n");
        return (0);
    }

    rtrn = pthread_create(&thread2, NULL, thread_start, pool);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create thread\n");
        return (0);
    }

    /* Retrieve some blocks, then return them. */
    for(i = 0; i < count; i++)
    {
        m_blk = mem_get_shared_block(pool);

        assert_stat(m_blk != NULL);
        assert_stat(m_blk->ptr != NULL);

        mem_free_shared_block(m_blk, pool);
    }

    void *buf = NULL;

    rtrn = pthread_join(thread1, &buf);
    if(rtrn < 0)
    {
        output(ERROR, "Can't join thread1\n");
        return (-1);
    }

    rtrn = pthread_join(thread2, &buf);
    if(rtrn < 0)
    {
        output(ERROR, "Can't join thread2\n");
        return (-1);
    }

    log_test(SUCCESS, "Shared memory pool test passed");

    return (0);
}

static int32_t test_mem_calloc(void)
{
    log_test(DECLARE, "Testing mem calloc");

    char *buffer = NULL;

    buffer = mem_calloc(1000);
    assert_stat(buffer != NULL);

    uint32_t i;

    for(i = 0; i < 1000; i++)
    {
        assert_stat(buffer[i] == 0);
    }

    log_test(SUCCESS, "Mem calloc test passed");

    return (0);
}

static int32_t test_mem_calloc_shared(void)
{
    log_test(DECLARE, "Testing mem calloc shared");

    char *buf = NULL;

    /* Make sure mem_calloc_shared() fails when passed a zero size. */
    buf = mem_calloc_shared(0);
    assert_stat(buf == NULL);

    /* Buf should not be NULL when passing a number
    greater than zero. */
    buf = mem_calloc_shared(10);
    assert_stat(buf != NULL);

    pid_t pid = 0;
    int32_t rtrn = 0;
    int32_t port[2];

    rtrn = pipe(port);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create pipe: %s\n", strerror(errno));
        return (-1);
    }

    pid = fork();
    if(pid == 0)
    {
        uint32_t i;

        for(i = 0; i < 10; i++)
        {
            assert_stat(buf[i] == 0);
        }

        write(port[1], "1", 1);

        _exit(0);
    }
    else if(pid > 0)
    {
        char *buf2 = mem_alloc(2);
        assert_stat(buf2 != NULL);

        /* Wait for the child to be done setting up. */
        ssize_t ret = read(port[0], buf2, 1);
        if(ret < 1)
        {
            mem_free((void **)&buf2);
            return -1;
        }

        mem_free((void **)&buf2);
    }
    else
    {
        output(ERROR, "Can't create child proc\n");
        return -1;
    }

    munmap(buf, 10);
    

    log_test(SUCCESS, "Mem calloc shared test passed");

    return 0;
}

int main(void)
{
    int32_t rtrn = 0;

    test_stat = init_test_framework();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't init test framework");
        return (-1);
    }

    rtrn = test_shared_pool();
    if(rtrn < 0)
        log_test(FAIL, "shared pool test failed");

    rtrn = test_mem_alloc();
    if(rtrn < 0)
        log_test(FAIL, "memory allocator test failed");

    rtrn = test_mem_free();
    if(rtrn < 0)
        log_test(FAIL, "Mem free test failed");

    rtrn = test_mem_free_shared();
    if(rtrn < 0)
        log_test(FAIL, "Mem free shared test failed");

    rtrn = test_mem_calloc();
    if(rtrn < 0)
        log_test(FAIL, "calloc test failed");

    rtrn = test_mem_calloc_shared();
    if(rtrn < 0)
        log_test(FAIL, "calloc shared test failed");

    rtrn = test_mem_alloc_shared();
    if(rtrn < 0)
        log_test(FAIL, "anonymous shared memory allocator test failed");

    _exit(0);
}


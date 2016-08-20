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

struct test_obj
{
    int32_t value;
    const char padding[4];
    char *ptr;
};

static void test_mem_free_shared(void)
{
    char *buffer = NULL;

    buffer = mem_alloc_shared(100);
    TEST_ASSERT_NOT_NULL(buffer);

    mem_free_shared((void **)&buffer, 100);
    TEST_ASSERT_NULL(buffer);

    return;
} 

static void test_mem_free(void)
{
    char *buffer = NULL;

    buffer = mem_alloc(100);
    TEST_ASSERT_NOT_NULL(buffer);

    mem_free((void **)&buffer);
    TEST_ASSERT_NULL(buffer);

    return;
}

static void test_mem_alloc(void)
{
    char *buf = NULL;

    /* Make sure mem_alloc() fails when passed a zero size. */
    buf = mem_alloc(0);
    TEST_ASSERT_NULL(buf);

    buf = mem_alloc(10);
    TEST_ASSERT_NOT_NULL(buf);

    mem_free((void **)&buf);
    TEST_ASSERT_NULL(buf);

    return;
}

static void test_mem_calloc(void)
{
    char *buf = NULL;

    /* Make sure mem_calloc() fails when passed a zero size. */
    buf = mem_calloc(0);
    TEST_ASSERT_NULL(buf);

    buf = mem_calloc(10);
    TEST_ASSERT_NOT_NULL(buf);

    uint32_t i;

    /* Make sure the memory was set to zero. */
    for(i = 0; i < 10; i++)
    {
        TEST_ASSERT(buf[i] == 0);
    }

    mem_free((void **)&buf);
    TEST_ASSERT_NULL(buf);

    return;
}

static void test_mem_alloc_shared(void)
{
    char *buf = NULL;
    pid_t pid;
    int rtrn;
    int port[2];

    /* Make sure mem_alloc_shared() fails when passed a zero size. */
    buf = mem_alloc_shared(0);
    TEST_ASSERT_NULL(buf);

    buf = mem_alloc_shared(10);
    TEST_ASSERT_NOT_NULL(buf);

    buf = "test";

    rtrn = pipe(port);
    TEST_ASSERT(rtrn > -1);

    pid = fork();
    if(pid == 0)
    {
        TEST_ASSERT(strncmp(buf, "test", 4) == 0);

        write(port[1], "1", 1);

        _exit(0);
    }
    else if(pid > 0)
    {
        char *buf2 = mem_alloc(2);
        TEST_ASSERT_NOT_NULL(buf2);

        /* Wait for the child to be done setting up. */
        ssize_t ret = read(port[0], buf2, 1);
        TEST_ASSERT(ret == 1)
    
        mem_free((void **)&buf2);
    }
    else
    {
        return;
    }

    mem_free_shared((void **)&buf, 10);

    return;
}

int main(void)
{
    test_mem_alloc_shared();
    test_mem_free_shared();
    test_mem_alloc();
    test_mem_free();
    test_mem_calloc();

	return (0);
}
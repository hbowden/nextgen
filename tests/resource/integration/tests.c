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
#include "crypto/crypto.h"
#include "resource/resource.c"
#include "concurrent/concurrent.h"

#include <pthread.h>

static uint32_t iterations = 10000000;

static void test_clean_fd_pool(void)
{
	return;
}

static void test_clean_dirpath_pool(void)
{
	return;
}

static void test_clean_socket_pool(void)
{
	return;
}

static void test_clean_file_pool(void)
{
    uint32_t i = 0;
    int32_t rtrn = 0;
    char **paths = NULL;

    /* Create an array of char pointers. */
    paths = mem_alloc(sizeof(char *) * POOL_SIZE);
    TEST_ASSERT_NOT_NULL(paths);

    /* Loop and allocate each index in the array. */
    for(i = 0; i < POOL_SIZE; i++)
    {
        paths[i] = mem_alloc(PATH_MAX + 1);
        TEST_ASSERT_NOT_NULL(paths[i])
    }

    struct memory_block *m_blk = NULL;

    /* Reset i back to zero. */
    i = 0;

    /* Loop and grab all the file paths in the allocated list. */
    NX_SLIST_FOREACH(m_blk, &file_pool->allocated_list)
    {
        struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

        /* Copy the path pointer into the paths index. */
        memcpy(paths[i], (char *)resource->ptr, strlen((char *)resource->ptr));

        i++;
    }

     /* Now check the free list for any resource blocks. */
    NX_SLIST_FOREACH(m_blk, &file_pool->free_list)
    {
        struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

        /* Copy the path pointer into the paths index. */
        memcpy(paths[i], (char *)resource->ptr, strlen((char *)resource->ptr));

        i++;
    }

    /* Now that we have an index of file paths, let's call clean_file_pool().
    Now we can manually check that clean_file_pool() actually got rid of the files. */
    rtrn = clean_file_pool();

    /* Should return zero. */
    TEST_ASSERT(rtrn == 0);

    /* Make sure there is no allocated block in the free list. */
    NX_SLIST_FOREACH(m_blk, &file_pool->free_list)
    {
        /* The memory block should be NULL. */
        TEST_ASSERT_NULL(m_blk);

        /* The resource pointer should also be NULL. */
        TEST_ASSERT_NULL(m_blk->ptr);

        /* As should the file path pointer. */
        TEST_ASSERT_NULL(((struct resource_ctx *)m_blk->ptr)->ptr);
    }

    /* Now check the allocated list. */
    NX_SLIST_FOREACH(m_blk, &file_pool->allocated_list)
    {
        /* The memory block should be NULL. */
        TEST_ASSERT_NULL(m_blk);

        /* The resource pointer should also be NULL. */
        TEST_ASSERT_NULL(m_blk->ptr);

        /* As should the file path pointer. */
        TEST_ASSERT_NULL(((struct resource_ctx *)m_blk->ptr)->ptr);
    }

    int32_t fd = 0;

    /* Now let's loop through the paths array and make sure the file
    referenced by the file path has been cleaned up. */
    for(i = 0; i < POOL_SIZE; i++)
    {
        fd = open(paths[i], O_RDONLY);

        /* Make sure the open call failed. */
        TEST_ASSERT(fd < 0);
    }

    /* Now let's clean up our mess. */
    for(i = 0; i < POOL_SIZE; i++)
    {
        mem_free((void **)&paths[i]);
    }

    mem_free((void **)&paths);

    return;
}

static void *socket_test_thread(void *arg)
{
    (void)arg;

    uint32_t i;
    int32_t socket = 0;

    for(i = 0; i < iterations; i++)
    {
        socket = get_socket();

        TEST_ASSERT(socket != 0);
        TEST_ASSERT(socket != -1);

        free_socket(&socket);
    }

    return (NULL);
}

static void test_get_socket(void)
{
    int32_t rtrn = 0;
    int32_t socket = 0;
    pthread_t thread = 0;

    rtrn = pthread_create(&thread, NULL, socket_test_thread, NULL);
    TEST_ASSERT(rtrn > -1);

    uint32_t i;

    for(i = 0; i < iterations; i++)
    {
        socket = get_socket();

        TEST_ASSERT(socket != 0);
        TEST_ASSERT(socket != -1);

        free_socket(&socket);
    }

    pthread_join(thread, NULL);

    return;
}

static void *mountpath_test_thread(void *arg)
{
    (void)arg;

    uint32_t i;
    char *mountpath = NULL;

    for(i = 0; i < iterations; i++)
    {
        mountpath = get_mountpath();

        TEST_ASSERT_NOT_NULL(mountpath);

        free_mountpath(&mountpath);
    }

    return (NULL);
}

static void test_get_mountpath(void)
{
    int32_t rtrn = 0;
    char *mountpath = NULL;
    pthread_t thread = 0;

    rtrn = pthread_create(&thread, NULL, mountpath_test_thread, NULL);
    TEST_ASSERT(rtrn > -1);

    uint32_t i;

    for(i = 0; i < iterations; i++)
    {
        mountpath = get_mountpath();

        TEST_ASSERT_NOT_NULL(mountpath);

        free_mountpath(&mountpath);
    }

    pthread_join(thread, NULL);

    return;
}

static void *dirpath_test_thread(void *arg)
{
    (void)arg;

    uint32_t i;
    char *dirpath = NULL;

    for(i = 0; i < iterations; i++)
    {
        dirpath = get_dirpath();

        TEST_ASSERT_NOT_NULL(dirpath);

        free_dirpath(&dirpath);
    }

    return (NULL);
}

static void test_get_dirpath(void)
{
    int32_t rtrn = 0;
    char *dirpath = NULL;
    pthread_t thread = 0;

    rtrn = pthread_create(&thread, NULL, dirpath_test_thread, NULL);
    TEST_ASSERT(rtrn > -1);

    uint32_t i;

    for(i = 0; i < iterations; i++)
    {
        dirpath = get_dirpath();

        TEST_ASSERT_NOT_NULL(dirpath);

        free_dirpath(&dirpath);
    }

    pthread_join(thread, NULL);

    return;
}

static void *file_test_thread(void *arg)
{
    (void)arg;

    uint32_t i;
    char *path = NULL;

    for(i = 0; i < iterations; i++)
    {
        path = get_filepath();

        TEST_ASSERT_NOT_NULL(path);

        free_filepath(&path);
    }

    return (NULL);
}

static void test_get_file(void)
{
    int32_t rtrn = 0;
    char *path = NULL;
    pthread_t thread = 0;

    rtrn = pthread_create(&thread, NULL, file_test_thread, NULL);
    TEST_ASSERT(rtrn > -1);

    uint32_t i;

    for(i = 0; i < iterations; i++)
    {
        path = get_filepath();
        TEST_ASSERT_NOT_NULL(path);

        free_filepath(&path);
    }

    pthread_join(thread, NULL);

    return;
}

static void *fd_test_thread(void *arg)
{
    (void)arg;

    uint32_t i;
    int32_t fd = 0;

    for(i = 0; i < iterations; i++)
    {
        fd = get_desc();
        TEST_ASSERT(fd != -1);
        TEST_ASSERT(fd != 0);

        free_desc(&fd);
    }

    return (NULL);
}

static void test_get_fd(void)
{
    int32_t fd = 0;
    int32_t rtrn = 0;
    pthread_t thread = 0;

    rtrn = pthread_create(&thread, NULL, fd_test_thread, NULL);
    TEST_ASSERT(rtrn > -1);

    uint32_t i;

    for(i = 0; i < iterations; i++)
    {
        fd = get_desc();

        TEST_ASSERT(fd != -1);
        TEST_ASSERT(fd != 0);

        free_desc(&fd);
    }

    pthread_join(thread, NULL);

    return;
}

static void test_setup_resource_module(enum rsrc_gen_type type)
{
    int32_t rtrn = 0;

    /* Build resource pool using the user's home path as the root path. */
    rtrn = setup_resource_module(type, "/tmp");

    /* setup_resource_module() should return zero on success. */
    TEST_ASSERT(rtrn == 0);

    return;
}

static void test_create_socket_pool(void)
{
    struct mem_pool_shared *pool = NULL;

    int32_t rtrn = 0;

    /* Start socket server. We use this to connect to, to create loopback sockets. */
    rtrn = setup_network_module(SOCKET_SERVER);
    TEST_ASSERT(rtrn > -1);

    pool = create_socket_pool();
    TEST_ASSERT_NOT_NULL(pool);

    struct memory_block *m_blk = NULL;

    NX_SLIST_FOREACH(m_blk, &pool->free_list)
    {
        TEST_ASSERT_NOT_NULL(m_blk);
        TEST_ASSERT_NOT_NULL(m_blk->ptr);
    }

    return;
}

static void test_create_file_pool(void)
{
    struct mem_pool_shared *pool = NULL;

    pool = create_file_pool("/tmp");
    TEST_ASSERT_NOT_NULL(pool);

    struct memory_block *m_blk = NULL;

    NX_SLIST_FOREACH(m_blk, &pool->free_list)
    {
        TEST_ASSERT_NOT_NULL(m_blk);
        TEST_ASSERT_NOT_NULL(m_blk->ptr);
    }

    return;
}

static void test_create_dirpath_pool(void)
{
    struct mem_pool_shared *pool = NULL;

    pool = create_dirpath_pool("/tmp");
    TEST_ASSERT_NOT_NULL(pool);

    struct memory_block *m_blk = NULL;

    NX_SLIST_FOREACH(m_blk, &pool->free_list)
    {
        TEST_ASSERT_NOT_NULL(m_blk);
        TEST_ASSERT_NOT_NULL(m_blk->ptr);
    }

    return;
}

static void test_create_fd_pool(void)
{
    struct mem_pool_shared *pool = NULL;

    pool = create_fd_pool("/tmp");
    TEST_ASSERT_NOT_NULL(pool);

    struct memory_block *m_blk = NULL;

    NX_SLIST_FOREACH(m_blk, &pool->free_list)
    {
        TEST_ASSERT_NOT_NULL(m_blk);
        TEST_ASSERT_NOT_NULL(m_blk->ptr);
    }

    return;
}

static void test_init_resource_ctx(void)
{
    int32_t rtrn = 0;
    struct resource_ctx *resource = NULL;

    rtrn = init_resource_ctx(&resource, sizeof(int32_t *));
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT_NOT_NULL(resource);

    return;
}

static void test_cached(void)
{
	  test_setup_resource_module(CACHE);
    test_get_file();
    test_get_fd();
    test_get_dirpath();
    test_get_socket();
    test_get_mountpath();
    test_clean_file_pool();
    test_clean_fd_pool();
    test_clean_dirpath_pool();
    test_clean_socket_pool();

	return;
}

static void test_no_cached(void)
{
	  test_setup_resource_module(NO_CACHE);
    test_get_file();
    test_get_fd();
    test_get_dirpath();
    test_get_socket();
    test_get_mountpath();
    test_clean_file_pool();
    test_clean_fd_pool();
    test_clean_dirpath_pool();
    test_clean_socket_pool();

    return;
}

int main(void)
{
    int32_t rtrn = 0;

    /* We have to init the crypto module first before using the resource module. */
    rtrn = setup_crypto_module(CRYPTO);
    TEST_ASSERT(rtrn > -1);

    test_init_resource_ctx();
    test_create_file_pool();
    test_create_dirpath_pool();
    test_create_socket_pool();
    test_create_fd_pool();
    test_cached();
    test_no_cached();

	  _exit(0);
}

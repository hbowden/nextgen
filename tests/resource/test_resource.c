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
#include "resource/resource.c"
#include "crypto/crypto.h"
#include "concurrent/concurrent.h"

#include <pthread.h>

static uint32_t iterations = 10000;

static int32_t test_clean_file_pool(void)
{
    log_test(DECLARE, "Testing clean_file_pool");

    uint32_t i = 0;
    int32_t rtrn = 0;
    char **paths = NULL;

    /* Create an array of char pointers. */
    paths = mem_alloc(sizeof(char *) * POOL_SIZE);
    if(paths == NULL)
    {
        output(ERROR, "Can't allocate array\n");
        return (-1);
    }

    /* Loop and allocate each index in the array. */
    for(i = 0; i < POOL_SIZE; i++)
    {
        paths[i] = mem_alloc(PATH_MAX + 1);
        if(paths[i] == NULL)
        {
            output(ERROR, "Can't allocate path index\n");
            return (-1);
        }
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
    assert_stat(rtrn == 0);

    /* Make sure there is no allocated block in the free list. */
    NX_SLIST_FOREACH(m_blk, &file_pool->free_list)
    {
        /* The memory block should be NULL. */
        assert_stat(m_blk == NULL);

        /* The resource pointer should also be NULL. */
        assert_stat(m_blk->ptr == NULL);

        /* As should the file path pointer. */
        assert_stat(((struct resource_ctx *)m_blk->ptr)->ptr == NULL);
    }

    /* Now check the allocated list. */
    NX_SLIST_FOREACH(m_blk, &file_pool->allocated_list)
    {
        /* The memory block should be NULL. */
        assert_stat(m_blk == NULL);

        /* The resource pointer should also be NULL. */
        assert_stat(m_blk->ptr == NULL);

        /* As should the file path pointer. */
        assert_stat(((struct resource_ctx *)m_blk->ptr)->ptr == NULL);
    }

    int32_t fd = 0;

    /* Now let's loop through the paths array and make sure the file
    referenced by the file path has been cleaned up. */
    for(i = 0; i < POOL_SIZE; i++)
    {
        fd = open(paths[i], O_RDONLY);

        /* Make sure the open call failed. */
        assert_stat(fd < 0);
    }

    /* Now let's clean up our mess. */
    for(i = 0; i < POOL_SIZE; i++)
    {
        mem_free((void **)&paths[i]);
    }

    mem_free((void **)&paths);

    log_test(SUCCESS, "clean_file_pool test passed");

    return (0);
}

static void *socket_test_thread(void *arg)
{
    (void)arg;

    uint32_t i;
    int32_t socket = 0;

    for(i = 0; i < iterations; i++)
    {
        socket = get_socket();

        assert_stat(socket != 0);

        free_socket(&socket);
    }

    return (NULL);
}

static int32_t test_get_socket(void)
{
    log_test(DECLARE, "Testing get socket");

    int32_t rtrn = 0;
    int32_t socket = 0;
    pthread_t thread = 0;

    rtrn = pthread_create(&thread, NULL, socket_test_thread, NULL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create test thread\n");
        return (-1);
    }

    uint32_t i;

    for(i = 0; i < iterations; i++)
    {
        socket = get_socket();

        assert_stat(socket != 0);

        free_socket(&socket);
    }

    pthread_join(thread, NULL);

    log_test(SUCCESS, "get socket test passed");

    return (0);
}

static void *mountpath_test_thread(void *arg)
{
    (void)arg;

    uint32_t i;
    char *mountpath = NULL;

    for(i = 0; i < iterations; i++)
    {
        mountpath = get_mountpath();

        assert_stat(mountpath != NULL);

        free_mountpath(&mountpath);
    }

    return (NULL);
}

static int32_t test_get_mountpath(void)
{
    log_test(DECLARE, "Testing get mountpath");

    int32_t rtrn = 0;
    char *mountpath = NULL;
    pthread_t thread = 0;

    rtrn = pthread_create(&thread, NULL, mountpath_test_thread, NULL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create test thread\n");
        return (-1);
    }

    uint32_t i;

    for(i = 0; i < iterations; i++)
    {
        mountpath = get_mountpath();

        assert_stat(mountpath != NULL);

        free_mountpath(&mountpath);
    }

    pthread_join(thread, NULL);

    log_test(SUCCESS, "get mountpath test passed");

    return (0);
}

static void *dirpath_test_thread(void *arg)
{
    (void)arg;

    uint32_t i;
    char *dirpath = NULL;

    for(i = 0; i < iterations; i++)
    {
        dirpath = get_dirpath();
        assert_stat(dirpath != NULL);

        free_dirpath(&dirpath);
    }

    return (NULL);
}

static int32_t test_get_dirpath(void)
{
    log_test(DECLARE, "Testing get_dirpath");

    int32_t rtrn = 0;
    char *dirpath = NULL;
    pthread_t thread = 0;

    rtrn = pthread_create(&thread, NULL, dirpath_test_thread, NULL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create test thread\n");
        return (-1);
    }

    uint32_t i;
    
    for(i = 0; i < iterations; i++)
    {
        dirpath = get_dirpath();
        assert_stat(dirpath != NULL);

        free_dirpath(&dirpath);
    }

    pthread_join(thread, NULL);

    log_test(SUCCESS, "get_dirpath test passed");

    return (0);
}

static void *file_test_thread(void *arg)
{
    (void)arg;

    uint32_t i;
    char *path = NULL;

    for(i = 0; i < iterations; i++)
    {
        path = get_filepath();

        assert_stat(path != NULL);

        free_filepath(&path);
    }

    return (NULL);
}

static int32_t test_get_file(void)
{
    log_test(DECLARE, "Testing get file path");

    int32_t rtrn = 0;
    char *path = NULL;
    pthread_t thread = 0;

    rtrn = pthread_create(&thread, NULL, file_test_thread, NULL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create test thread\n");
        return (-1);
    }

    uint32_t i;
    
    for(i = 0; i < iterations; i++)
    {
        path = get_filepath();
        assert_stat(path != NULL);

        free_filepath(&path);
    }

    pthread_join(thread, NULL);

    log_test(SUCCESS, "Get file path test passed");

    return (0);
}

static void *fd_test_thread(void *arg)
{
    (void)arg;

    uint32_t i;
    int32_t fd = 0;

    for(i = 0; i < iterations; i++)
    {
        fd = get_desc();

        assert_stat(fd != 0);

        free_socket(&fd);
    }

    return (NULL);
}

static int32_t test_get_fd(void)
{
	log_test(DECLARE, "Testing get file descriptor");

    int32_t fd = 0;
    int32_t rtrn = 0;
    pthread_t thread = 0;

    rtrn = pthread_create(&thread, NULL, fd_test_thread, NULL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create test thread\n");
        return (-1);
    }

    uint32_t i;
    
    for(i = 0; i < iterations; i++)
    {
        fd = get_desc();

        assert_stat(fd > 0);

        free_desc(&fd);
    }

    pthread_join(thread, NULL);
	
    log_test(SUCCESS, "Get file descriptor test passed");

    return (0);
}

static int32_t test_setup_resource_module(void)
{
    log_test(DECLARE, "Testing resource module setup");

    int32_t rtrn = 0;

    /* Build resource pool using the user's home path as the root path. */
    rtrn = setup_resource_module("/tmp");

    /* setup_resource_module() should return zero on success. */
    assert(rtrn == 0);

    log_test(SUCCESS, "Resource module setup test passed");

    return (0);
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

    /* We have to init the crypto module first before using the resource module. */
    rtrn = setup_crypto_module(CRYPTO);
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup crypto module\n");
        return (0);
    }

    rtrn = test_setup_resource_module();
    if(rtrn < 0)
        log_test(FAIL, "setup resource module failed");

	rtrn = test_get_fd();
	if(rtrn < 0)
        log_test(FAIL, "get fd test failed");

    rtrn = test_get_file();
    if(rtrn < 0)
        log_test(FAIL, "Get file path test failed");

    rtrn = test_get_dirpath();
    if(rtrn < 0)
        log_test(FAIL, "get dirpath test failed");

    rtrn = test_get_socket();
    if(rtrn < 0)
        log_test(FAIL, "get socket failed");

    rtrn = test_get_mountpath();
    if(rtrn < 0)
        log_test(FAIL, "Get mountpath test failed");

    rtrn = test_clean_file_pool();
    if(rtrn < 0)
        log_test(FAIL, "clean_file_pool test failed");

	_exit(0);
}

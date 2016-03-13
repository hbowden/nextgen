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

#include "resource.h"
#include "concurrent/concurrent.h"
#include "crypto/crypto.h"
#include "io/io.h"
#include "network/network.h"
#include "platform.h"
#include "utils/utils.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

static struct mem_pool_shared *desc_pool;

static struct mem_pool_shared *mount_pool;

static struct mem_pool_shared *dirpath_pool;

static struct mem_pool_shared *file_pool;

static struct mem_pool_shared *socket_pool;

/* The resource free_* functions are inefficient and should be replaced
 with a more efficent implementation. */
int32_t get_desc(void)
{
    int32_t *fd = NULL;
    struct memory_block *m_blk = NULL;

    /* Grab a shared memory block from the decriptor pool. */
    m_blk = mem_get_shared_block(desc_pool);
    if(m_blk == NULL)
    {
        output(ERROR, "Can't get shared block\n");
        return (-1);
    }

    /* Get resource pointer. */
    struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

    /* Set fd to the descriptor we grabbed from the pool. */
    fd = (int32_t *)resource->ptr;

    return (*fd);
}

int32_t free_desc(int32_t *fd)
{
    /* Declare a memory block pointer. */
    struct memory_block *m_blk = NULL;

    /* Loop and find the resource */
    NX_SLIST_FOREACH(m_blk, &desc_pool->allocated_list)
    {
        /* Get resource pointer. */
        struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

        /* Get the descriptor pointer. */
        int32_t *desc = (int32_t *)resource->ptr;

        /* If the descriptors match, free the memory block.*/
        if((*fd) == (*desc))
        {
            mem_free_shared_block(m_blk, desc_pool);
            break;
        }
    }

    return (0);
}

int32_t get_socket(void)
{
    int32_t *sock = NULL;
    struct memory_block *m_blk = NULL;

    /* Grab a shared memory block from the socket pool. */
    m_blk = mem_get_shared_block(socket_pool);
    if(m_blk == NULL)
    {
        output(ERROR, "Can't get shared block\n");
        return (-1);
    }

    /* Get resource pointer. */
    struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

    /* Set fd to the descriptor we grabbed from the pool. */
    sock = (int32_t *)resource->ptr;

    return ((*sock));
}

int32_t free_socket(int32_t *sock_fd)
{
    /* Declare a memory block pointer. */
    struct memory_block *m_blk = NULL;

    /* Loop and find the resource */
    NX_SLIST_FOREACH(m_blk, &socket_pool->allocated_list)
    {
        /* Get resource pointer. */
        struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

        /* Get the socket pointer. */
        int32_t *sock = (int32_t *)resource->ptr;

        /* If the descriptors match, free the memory block.*/
        if((*sock) == (*sock_fd))
        {
            mem_free_shared_block(m_blk, socket_pool);
            break;
        }
    }

    return (0);
}

char *get_mountpath(void)
{
    char *path = NULL;
    struct memory_block *m_blk = NULL;

    /* Grab a shared memory block from the mountpath pool. */
    m_blk = mem_get_shared_block(mount_pool);
    if(m_blk == NULL)
    {
        output(ERROR, "Can't get shared block\n");
        return NULL;
    }

    /* Get resource pointer. */
    struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

    path = (char *)resource->ptr;

    return (path);
}

int32_t free_mountpath(char **path)
{
    /* Declare a memory block pointer. */
    struct memory_block *m_blk = NULL;

    /* Loop and find the resource */
    NX_SLIST_FOREACH(m_blk, &mount_pool->allocated_list)
    {
        /* Get resource pointer. */
        struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

        /* Get the mountpath pointer. */
        char *dirpath = (char *)resource->ptr;

        /* If the dirpaths match, free the memory block.*/
        if(strcmp((*path), dirpath) == 0)
        {
            mem_free_shared_block(m_blk, mount_pool);
            break;
        }
    }

    return (0);
}

char *get_dirpath(void)
{
    char *path = NULL;
    struct memory_block *m_blk = NULL;

    /* Grab a shared memory block from the dirpath pool. */
    m_blk = mem_get_shared_block(dirpath_pool);
    if(m_blk == NULL)
    {
        output(ERROR, "Can't get shared block\n");
        return NULL;
    }

    /* Get resource pointer. */
    struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

    /* Set fd to the descriptor we grabbed from the pool. */
    path = (char *)resource->ptr;

    return (path);
}

int32_t free_dirpath(char **path)
{
    /* Declare a memory block pointer. */
    struct memory_block *m_blk = NULL;

    /* Loop and find the resource */
    NX_SLIST_FOREACH(m_blk, &dirpath_pool->allocated_list)
    {
        /* Get resource pointer. */
        struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

        /* Get the dirpath pointer. */
        char *dirpath = (char *)resource->ptr;

        /* If the dirpaths match, free the memory block.*/
        if(strcmp((*path), dirpath) == 0)
        {
            mem_free_shared_block(m_blk, dirpath_pool);
            break;
        }
    }

    return (0);
}

char *get_filepath(void)
{
    char *path = NULL;
    struct memory_block *m_blk = NULL;

    /* Grab a shared memory block from the file pool. */
    m_blk = mem_get_shared_block(file_pool);
    if(m_blk == NULL)
    {
        output(ERROR, "Can't get shared block\n");
        return (NULL);
    }

    /* Get resource pointer. */
    struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

    /* Set fd to the descriptor we grabbed from the pool. */
    path = (char *)resource->ptr;

    return (path);
}

int32_t free_filepath(char **path)
{
    /* Declare a memory block pointer. */
    struct memory_block *m_blk = NULL;

    /* Loop and find the resource */
    NX_SLIST_FOREACH(m_blk, &file_pool->allocated_list)
    {
        /* Get resource pointer. */
        struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

        /* Get the filepath pointer. */
        char *filepath = (char *)resource->ptr;

        /* If the filepaths match, free the memory block.*/
        if(strncmp((*path), filepath, strlen(filepath)) == 0)
        {
            mem_free_shared_block(m_blk, file_pool);
            break;
        }
    }

    return (0);
}

static int32_t create_fd_pool(char *path)
{
    int32_t rtrn = 0;
    struct memory_block *m_blk = NULL;

    desc_pool = mem_create_shared_pool(sizeof(int32_t *), POOL_SIZE);
    if(desc_pool == NULL)
    {
        output(ERROR, "Can't allocate descriptor memory pool\n");
        return -1;
    }

    /* Create a bunch of file descriptors and stick them into the resource pool. */
    init_shared_pool(&desc_pool, m_blk)
    {
        /* Temp variables that we define with auto_clean so that we 
        don't have to worry about calling free. */
        char *file_name auto_free = NULL;
        char *junk auto_free = NULL;
        char *file_path auto_free = NULL;

        /* Don't free that will be taken cared off later. */
        struct resource_ctx *resource = NULL;

        resource = mem_alloc_shared(sizeof(struct resource_ctx));
        if(resource == NULL)
        {
            output(ERROR, "Can't alloc resource object\n");
            return (-1);
        }

        resource->ptr = mem_alloc_shared(sizeof(int32_t *));
        if(resource->ptr == NULL)
        {
            output(ERROR, "Can't alloc resource pointer\n");
            return (-1);
        }

        /* Use generate_name to create a random file name with the text extension.  */
        rtrn = generate_name((char **)&file_name, ".txt", FILE_NAME);
        if(rtrn < 0)
        {
            output(ERROR, "Can't generate file path\n");
            return (-1);
        }

        /* Join the file name with the home path. */
        rtrn = asprintf(&file_path, "%s/%s", path, file_name);
        if(rtrn < 0)
        {
            output(ERROR, "Can't join paths: %s\n", strerror(errno));
            return (-1);
        }

        /* Allocate a buffer to put junk in. */
        junk = mem_alloc(4096);
        if(junk == NULL)
        {
            output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
            return (-1);
        }

        /* Put some junk in a buffer. */
        rtrn = rand_bytes(&junk, 4095);
        if(rtrn < 0)
        {
            output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
            return (-1);
        }

        /* Write that junk to the file so that the file is not just blank. */
        rtrn = map_file_out(file_path, junk, 4095);
        if(rtrn < 0)
        {
            output(ERROR, "Can't write junk to disk\n");
            return -1;
        }

        int32_t fd = open(file_path, O_RDWR, 0777);
        if(fd < 0)
        {
            output(ERROR, "Can't open newly created file: %s\n",
                   strerror(errno));
            return -1;
        }

        memmove(resource->ptr, &fd, sizeof(int32_t));

        resource->m_blk = m_blk;

        m_blk->ptr = resource;
    }

    return (0);
}

static int32_t create_file_pool(char *path)
{
    int32_t rtrn = 0;
    struct memory_block *m_blk = NULL;

    file_pool = mem_create_shared_pool(sizeof(char *), POOL_SIZE);
    if(file_pool == NULL)
    {
        output(ERROR, "Can't allocate file path memory pool\n");
        return (-1);
    }

    init_shared_pool(&file_pool, m_blk)
    {
        /* Temp variables that we define with auto_clean so that we 
        don't have to worry about calling free. */
        char *file_name auto_free = NULL;
        char *junk auto_free = NULL;

        /* Don't free, that will be taken cared off later. */
        char *file_path = NULL;
        struct resource_ctx *resource = NULL;

        resource = mem_alloc_shared(sizeof(struct resource_ctx));
        if(resource == NULL)
        {
            output(ERROR, "Can't alloc resource object\n");
            return (-1);
        }

        resource->ptr = mem_alloc_shared(PATH_MAX + 1);
        if(resource->ptr == NULL)
        {
            output(ERROR, "Can't alloc resource pointer\n");
            return (-1);
        }

        /* Use generate_name to create a random file name with the text extension.  */
        rtrn = generate_name((char **)&file_name, ".txt", FILE_NAME);
        if(rtrn < 0)
        {
            output(ERROR, "Can't generate file path\n");
            return (-1);
        }

        /* Join the file name with the home path. */
        rtrn = asprintf(&file_path, "%s/%s", path, file_name);
        if(rtrn < 0)
        {
            output(ERROR, "Can't join paths: %s\n", strerror(errno));
            return -1;
        }

        /* Allocate a buffer to put junk in. */
        junk = mem_alloc(4096);
        if(junk == NULL)
        {
            output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
            return -1;
        }

        /* Put some junk in a buffer. */
        rtrn = rand_bytes(&junk, 4095);
        if(rtrn < 0)
        {
            output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
            return -1;
        }

        /* Write that junk to the file so that the file is not just blank. */
        rtrn = map_file_out(file_path, junk, 4095);
        if(rtrn < 0)
        {
            output(ERROR, "Can't write junk to disk\n");
            return -1;
        }

        memmove(resource->ptr, file_path, strlen(file_path));

        resource->m_blk = m_blk;

        m_blk->ptr = resource;
    }
    return (0);
}

/*static int32_t create_mount_pool(char *path)
{
	mount_pool = mem_create_shared_pool(sizeof(char *), POOL_SIZE);
	if(mount_pool == NULL)
	{
		output(ERROR, "Can't allocate mount path memory pool\n");
		return (-1);
	}

    int32_t rtrn = 0;
	struct memory_block *block = NULL;

    SLIST_FOREACH(block, &mount_pool->free_list, list_entry)
    {
        char *fs_name auto_clean = NULL;

        rtrn = generate_name((char **)&fs_name, NULL, DIR_NAME);
        if(rtrn < 0)
        {
            output(ERROR, "Can't generate dir name\n");
            return (-1);
        }

        char *name = NULL;

        name = mem_alloc(9);
        if(name == NULL)
        {
            output(ERROR, "Can't alloc name buffer\n");
            return (-1);
        }

        memcpy(name, fs_name, 8);

        output(ERROR, "name: %s\n", name);
        
        rtrn = mount(name, path, MNT_FORCE, NULL);
        if(rtrn < 0)
        {
           output(ERROR, "mount: %s\n", strerror(errno));
           return (-1);
        }

        block->ptr = name;
    }

	return (0);
} */

static int32_t create_dirpath_pool(char *path)
{
    dirpath_pool = mem_create_shared_pool(sizeof(char *), POOL_SIZE);
    if(dirpath_pool == NULL)
    {
        output(ERROR, "Can't allocate dir path memory pool\n");
        return (-1);
    }

    int32_t rtrn = 0;
    struct memory_block *m_blk = NULL;

    NX_SLIST_FOREACH(m_blk, &dirpath_pool->free_list)
    {
        /* Temp variable that we define with auto_clean so that we 
        don't have to worry about calling free. */
        char *dir_name auto_free = NULL;

        /* Don't free, that will be taken cared of later. */
        char *dir_path = NULL;
        struct resource_ctx *resource = NULL;

        resource = mem_alloc_shared(sizeof(struct resource_ctx));
        if(resource == NULL)
        {
            output(ERROR, "Can't alloc resource object\n");
            return (-1);
        }

        resource->ptr = mem_alloc_shared(PATH_MAX + 1);
        if(resource->ptr == NULL)
        {
            output(ERROR, "Can't alloc resource pointer\n");
            return (-1);
        }

        /* Use generate_name to create a random directory name.  */
        rtrn = generate_name((char **)&dir_name, NULL, DIR_NAME);
        if(rtrn < 0)
        {
            output(ERROR, "Can't generate file path\n");
            return -1;
        }

        /* Join the dir name with the temp path. */
        rtrn = asprintf(&dir_path, "%s/%s", path, dir_name);
        if(rtrn < 0)
        {
            output(ERROR, "Can't join paths: %s\n", strerror(errno));
            return -1;
        }

        memmove(resource->ptr, dir_path, strlen(dir_path));

        resource->m_blk = m_blk;

        m_blk->ptr = resource;
    }

    return (0);
}

static int32_t clean_file_pool(void)
{
    int32_t rtrn = 0;
    struct memory_block *m_blk = NULL;

    if(file_pool == NULL)
    {
        output(ERROR, "File pool already clean\n");
        return (-1);
    }

    /* Lock the spinlock. */
    nx_spinlock_lock(&file_pool->lock);

    /* Loop and grab all the file paths in the allocated list. */
    NX_SLIST_FOREACH(m_blk, &file_pool->allocated_list)
    {
        struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

        rtrn = unlink((char *)resource->ptr);
        if(rtrn < 0)
        {
            output(ERROR, "Can't remove temp file\n");
            return (-1);
        }

        mem_free_shared((void **)&resource->ptr, PATH_MAX + 1);
        mem_free_shared((void **)&m_blk->ptr, sizeof(struct resource_ctx));
        mem_free_shared((void **)&m_blk, sizeof(struct memory_block));
    }

    /* Now check the free list for any resource blocks. */
    NX_SLIST_FOREACH(m_blk, &file_pool->free_list)
    {
        struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

        rtrn = unlink((char *)resource->ptr);
        if(rtrn < 0)
        {
            output(ERROR, "Can't remove temp file\n");
            return (-1);
        }

        mem_free_shared((void **)&resource->ptr, PATH_MAX + 1);
        mem_free_shared((void **)&m_blk->ptr, sizeof(struct resource_ctx));
        mem_free_shared((void **)&m_blk, sizeof(struct memory_block));
    }

    nx_spinlock_unlock(&file_pool->lock);

    mem_clean_shared_pool(file_pool);

    return (0);
}

static int32_t create_socket_pool(void)
{
    socket_pool = mem_create_shared_pool(sizeof(int32_t), POOL_SIZE);
    if(socket_pool == NULL)
    {
        output(ERROR, "Can't allocate socket memory pool\n");
        return (-1);
    }

    int32_t rtrn = 0;
    struct memory_block *m_blk = NULL;

    NX_SLIST_FOREACH(m_blk, &socket_pool->free_list)
    {
        int32_t *sock = NULL;

        struct resource_ctx *resource = NULL;

        resource = mem_alloc_shared(sizeof(struct resource_ctx));
        if(resource == NULL)
        {
            output(ERROR, "Can't alloc resource object\n");
            return (-1);
        }

        resource->ptr = mem_alloc_shared(sizeof(int32_t));
        if(resource->ptr == NULL)
        {
            output(ERROR, "Can't alloc resource pointer\n");
            return (-1);
        }

        sock = mem_alloc(sizeof(int32_t));
        if(sock == NULL)
        {
            output(ERROR, "Can't allocate socket\n");
            return (-1);
        }

        rtrn = connect_ipv6(sock);
        if(rtrn < 0)
        {
            output(ERROR, "Can't create socket\n");
            return (-1);
        }

        memmove(resource->ptr, sock, sizeof(int32_t));

        resource->m_blk = m_blk;

        m_blk->ptr = resource;
    }

    return (0);
}

int32_t cleanup_resource_pool(void)
{
    int32_t rtrn = 0;

    rtrn = clean_file_pool();
    if(rtrn < 0)
    {
        output(ERROR, "Can't clean file pool\n");
        return (-1);
    }

    return (0);
}

int32_t setup_resource_module(char *path)
{
    output(STD, "Creating resource pools\n");

    int32_t rtrn = 0;

    /* Start socket server. We use this to connect to, to create loopback sockets. */
    rtrn = setup_network_module(SOCKET_SERVER);
    if(rtrn < 0)
    {
        output(ERROR, "Can't start socket server\n");
        return (-1);
    }

    /*	rtrn = create_mount_pool(path);
	if(rtrn < 0)
	{
		output(ERROR, "Can't create mount pool\n");
		return -1;
	} */

    rtrn = create_file_pool(path);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create file pool\n");
        return (-1);
    }

    rtrn = create_socket_pool();
    if(rtrn < 0)
    {
        output(ERROR, "Can't create socket pool\n");
        return (-1);
    }

    rtrn = create_fd_pool(path);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create fd pool\n");
        return (-1);
    }

    rtrn = create_dirpath_pool(path);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create fd pool\n");
        return (-1);
    }

    return (0);
}

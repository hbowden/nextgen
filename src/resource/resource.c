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

#ifdef LINUX
/* We need to define _GNU_SOURCE to use
 asprintf on Linux. We also need to place
 _GNU_SOURCE at the top of the file before
 any other includes for it to work properly. */
#define _GNU_SOURCE

#endif

#include "resource.h"
#include "concurrent/concurrent.h"
#include "crypto/crypto.h"
#include "network/network.h"
#include "runtime/platform.h"
#include "utils/utils.h"
#include "utils/autofree.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

/* Shared memory pools. */
// static struct mem_pool_shared *desc_pool;
// static struct mem_pool_shared *mount_pool;
// static struct mem_pool_shared *dirpath_pool;
// static struct mem_pool_shared *file_pool;
// static struct mem_pool_shared *socket_pool;

// static char *get_dirpath_nocached(void)
// {
//     int32_t rtrn = 0;
//     char *path = NULL;
//     struct output_writter *output = NULL;
//
//     /* Just grab the console writter in here so we don't break
//      the old api but can use the new create_random_directory() interface. */
//     output = get_console_writter();
//     if(output == NULL)
//     {
//         printf("Failed to get console writter\n");
//         return (NULL);
//     }
//
//     rtrn = create_random_directory("/tmp", &path, output);
//     if(rtrn < 0)
//     {
//         output->write(ERROR, "Can't create directory");
//         return (NULL);
//     }
//
//     return (path);
// }
//
// static char *get_filepath_nocached(void)
// {
//     char *path = NULL;
//     int32_t rtrn = 0;
//     uint64_t size = 0;
//     struct output_writter *output = NULL;
//     struct memory_allocator *allocator = NULL;
//     struct random_generator *random = NULL;
//
//     /* Just grab the console writter, default allocator, and random generator
//       in here so we don't break the old api but can use the new create_random_file() interface. */
//     output = get_console_writter();
//     if(output == NULL)
//     {
//         printf("Failed to get console writter\n");
//         return (NULL);
//     }
//
//     allocator = get_default_allocator();
//     if(allocator == NULL)
//     {
//         output->write(ERROR, "Failed to get default memory allocator\n");
//         return (NULL);
//     }
//
//     random = get_default_random_generator(allocator, output);
//     if(random == NULL)
//     {
//         output->write(ERROR, "Failed to get random number generator\n");
//         return (NULL);
//     }
//
//     rtrn = create_random_file("/tmp", ".txt", &path, &size, random, allocator, output);
//     if(rtrn < 0)
//     {
//         output->write(ERROR, "Can't create random file\n");
//         return (NULL);
//     }
//
//     (void)size;
//
//     return (path);
// }
//
// static char *get_mountpath_nocached(void)
// {
//     char *path = NULL;
//
//     return (path);
// }
//
// static int32_t get_socket_nocached(void)
// {
//     int32_t rtrn = 0;
//     uint32_t num = 0;
//     int32_t sock = 0;
//
//     struct output_writter *output = NULL;
//     struct memory_allocator *allocator = NULL;
//     struct random_generator *random = NULL;
//
//     /* Just grab the console writter, default allocator, and random generator
//       in here so we don't break the old api but can use the new random->range() interface. */
//     output = get_console_writter();
//     if(output == NULL)
//     {
//         printf("Failed to get console writter\n");
//         return (-1);
//     }
//
//     allocator = get_default_allocator();
//     if(allocator == NULL)
//     {
//         output->write(ERROR, "Failed to get default memory allocator\n");
//         return (-1);
//     }
//
//     random = get_default_random_generator(allocator, output);
//     if(random == NULL)
//     {
//         output->write(ERROR, "Failed to get random number generator\n");
//         return (-1);
//     }
//
//     rtrn = random->range(1, &num);
//     if(rtrn < 0)
//     {
//         output->write(ERROR, "Can't generate random number\n");
//         return (-1);
//     }
//
//     switch(num)
//     {
//         case 0:
//             connect_ipv6(&sock);
//             break;
//
//         case 1:
//             connect_ipv4(&sock);
//             break;
//
//         default:
//             output->write(ERROR, "Should not get here\n");
//             return (-1);
//     }
//
//     return (sock);
// }
//
static int32_t get_desc_nocached(struct memory_allocator *allocator,
                                 struct output_writter *output,
                                 struct random_generator *random)
{
    int32_t fd = 0;
    int32_t rtrn = 0;
    uint64_t size = 0;
    char *path auto_free = NULL;

    rtrn = create_random_file("/tmp", ".txt", &path, &size, random, allocator, output);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't create random file\n");
        return (-1);
    }

    /* Open the newly created file, there's no need for
    closing the descriptor that is taken cared of latter. */
    fd = open(path, O_RDWR);
    if(fd < 0)
    {
        output->write(ERROR, "Can't open file: %s", strerror(errno));
        return (-1);
    }

    return (fd);
}
//
// static int32_t free_filepath_nocached(char **path)
// {
//     if((*path) == NULL)
//         return (-1);
//
//     int32_t rtrn = 0;
//     struct memory_allocator *allocator = NULL;
//     struct output_writter *output = NULL;
//
//     /* Just grab the console writter, default allocator, and random generator
//       in here so we don't break the old api. */
//     output = get_console_writter();
//     if(output == NULL)
//     {
//         printf("Failed to get console writter\n");
//         return (-1);
//     }
//
//     allocator = get_default_allocator();
//     if(allocator == NULL)
//     {
//         output->write(ERROR, "Failed to get default memory allocator\n");
//         return (-1);
//     }
//
//     /* Remove the file. */
//     rtrn = unlink((*path));
//     if(rtrn < 0)
//     {
//         output->write(ERROR, "Can't remove file: %s\n", strerror(errno));
//         return (-1);
//     }
//
//     /* Now free the memory used for the file path. */
//     allocator->free((void **)path);
//
//     return (0);
// }
//
// static int32_t free_mountpath_nocached(char **path)
// {
//     if((*path) == NULL)
//         return (-1);
//
//     /* Implement later. */
//     (void)(*path);
//
//     return (0);
// }
//
// static int32_t free_dirpath_nocached(char **path)
// {
//     if((*path) == NULL)
//         return (-1);
//
//     int32_t rtrn = 0;
//     struct memory_allocator *allocator = NULL;
//     struct output_writter *output = NULL;
//
//     /* Just grab the console writter and default allocator
//       in here so we don't break the old api. */
//     output = get_console_writter();
//     if(output == NULL)
//     {
//         printf("Failed to get console writter\n");
//         return (-1);
//     }
//
//     allocator = get_default_allocator();
//     if(allocator == NULL)
//     {
//         output->write(ERROR, "Failed to get default memory allocator\n");
//         return (-1);
//     }
//
//     rtrn = delete_directory((*path));
//     if(rtrn < 0)
//     {
//         output->write(ERROR, "Can't delete directory");
//         return (-1);
//     }
//
//     allocator->free((void **)path);
//
//     return (0);
// }
//
// static int32_t free_socket_nocached(int32_t *socket_fd)
// {
//     close((*socket_fd));
//
//     return (0);
// }
//
static int32_t free_desc_nocached(int32_t *fd)
{
    close((*fd));

    return (0);
}
//
// static int32_t free_filepath_cached(char **path)
// {
//     /* Declare a memory block pointer. */
//     struct memory_block *m_blk = NULL;
//
//     /* Loop and find the resource */
//     NX_SLIST_FOREACH(m_blk, &file_pool->allocated_list)
//     {
//         /* Get resource pointer. */
//         struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;
//
//         /* Get the filepath pointer. */
//         char *filepath = (char *)resource->ptr;
//
//         /* If the filepaths match, free the memory block.*/
//         if(memcmp((*path), filepath, strlen(filepath)) == 0)
//         {
//             mem_free_shared_block(m_blk, file_pool);
//             break;
//         }
//     }
//
//     return (0);
// }
//
// static int32_t free_mountpath_cached(char **path)
// {
//     /* Declare a memory block pointer. */
//     struct memory_block *m_blk = NULL;
//
//     /* Loop and find the resource */
//     NX_SLIST_FOREACH(m_blk, &mount_pool->allocated_list)
//     {
//         /* Get resource pointer. */
//         struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;
//
//         /* Get the mountpath pointer. */
//         char *dirpath = (char *)resource->ptr;
//
//         /* If the dirpaths match, free the memory block.*/
//         if(strcmp((*path), dirpath) == 0)
//         {
//             mem_free_shared_block(m_blk, mount_pool);
//             break;
//         }
//     }
//
//     return (0);
// }
//
// static int32_t free_dirpath_cached(char **path)
// {
//     /* Declare a memory block pointer. */
//     struct memory_block *m_blk = NULL;
//
//     /* Loop and find the resource */
//     NX_SLIST_FOREACH(m_blk, &dirpath_pool->allocated_list)
//     {
//         /* Get resource pointer. */
//         struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;
//
//         /* Get the dirpath pointer. */
//         char *dirpath = (char *)resource->ptr;
//
//         /* If the dirpaths match, free the memory block.*/
//         if(strcmp((*path), dirpath) == 0)
//         {
//             mem_free_shared_block(m_blk, dirpath_pool);
//             break;
//         }
//     }
//
//     return (0);
// }
//
// static int32_t free_socket_cached(int32_t *sock_fd)
// {
//     /* Declare a memory block pointer. */
//     struct memory_block *m_blk = NULL;
//
//     /* Loop and find the resource */
//     NX_SLIST_FOREACH(m_blk, &socket_pool->allocated_list)
//     {
//         /* Get resource pointer. */
//         struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;
//
//         /* Get the socket pointer. */
//         int32_t *sock = (int32_t *)resource->ptr;
//
//         /* If the descriptors match, free the memory block.*/
//         if((*sock) == (*sock_fd))
//         {
//             mem_free_shared_block(m_blk, socket_pool);
//             break;
//         }
//     }
//
//     return (0);
// }
//
// static int32_t free_desc_cached(int32_t *fd)
// {
//     /* Declare a memory block pointer. */
//     struct memory_block *m_blk = NULL;
//
//     /* Loop and find the resource */
//     NX_SLIST_FOREACH(m_blk, &desc_pool->allocated_list)
//     {
//         /* Get resource pointer. */
//         struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;
//
//         /* Get the descriptor pointer. */
//         int32_t *desc = (int32_t *)resource->ptr;
//
//         /* If the descriptors match, free the memory block.*/
//         if((*fd) == (*desc))
//         {
//             mem_free_shared_block(m_blk, desc_pool);
//             break;
//         }
//     }
//
//     return (0);
// }
//
// static char *get_dirpath_cached(void)
// {
//     char *path = NULL;
//     struct memory_block *m_blk = NULL;
//
//     /* Grab a shared memory block from the dirpath pool. */
//     m_blk = mem_get_shared_block(dirpath_pool);
//     if(m_blk == NULL)
//     {
//         printf("Can't get shared block\n");
//         return NULL;
//     }
//
//     /* Get resource pointer. */
//     struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;
//
//     /* Set fd to the descriptor we grabbed from the pool. */
//     path = (char *)resource->ptr;
//
//     return (path);
// }
//
// static char *get_filepath_cached(void)
// {
//     char *path = NULL;
//     struct memory_block *m_blk = NULL;
//
//     /* Grab a shared memory block from the file pool. */
//     m_blk = mem_get_shared_block(file_pool);
//     if(m_blk == NULL)
//     {
//         printf("Can't get shared block\n");
//         return (NULL);
//     }
//
//     /* Get resource pointer. */
//     struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;
//
//     /* Set fd to the descriptor we grabbed from the pool. */
//     path = (char *)resource->ptr;
//
//     return (path);
//
// }
//
// static char *get_mountpath_cached(void)
// {
//     char *path = NULL;
//     struct memory_block *m_blk = NULL;
//
//     /* Grab a shared memory block from the mountpath pool. */
//     m_blk = mem_get_shared_block(mount_pool);
//     if(m_blk == NULL)
//     {
//         printf("Can't get shared block\n");
//         return NULL;
//     }
//
//     /* Get resource pointer. */
//     struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;
//
//     path = (char *)resource->ptr;
//
//     return (path);
// }
//
// static int32_t get_socket_cached(void)
// {
//     int32_t *sock = NULL;
//     struct memory_block *m_blk = NULL;
//
//     /* Grab a shared memory block from the socket pool. */
//     m_blk = mem_get_shared_block(socket_pool);
//     if(m_blk == NULL)
//     {
//         printf("Can't get shared block\n");
//         return (-1);
//     }
//
//     /* Get resource pointer. */
//     struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;
//
//     /* Set fd to the descriptor we grabbed from the pool. */
//     sock = (int32_t *)resource->ptr;
//
//     return ((*sock));
// }
//
// static int32_t get_desc_cached(void)
// {
//     int32_t *fd = NULL;
//     struct memory_block *m_blk = NULL;
//
//     /* Grab a shared memory block from the decriptor pool. */
//     m_blk = mem_get_shared_block(desc_pool);
//     if(m_blk == NULL)
//     {
//         printf("Can't get shared block\n");
//         return (-1);
//     }
//
//     /* Get resource pointer. */
//     struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;
//
//     /* Set fd to the descriptor we grabbed from the pool. */
//     fd = (int32_t *)resource->ptr;
//
//     return (*fd);
// }
//
// static int32_t init_resource_ctx(struct resource_ctx **resource, uint32_t size)
// {
//     struct memory_allocator *allocator = NULL;
//     allocator = get_default_allocator();
//     if(allocator == NULL)
//     {
//         printf("Failed to get default memory allocator\n");
//         return (-1);
//     }
//
//     /* Allocate a resource context struct as shared memory. */
//     (*resource) = allocator->shared(sizeof(struct resource_ctx));
//     if((*resource) == NULL)
//     {
//         printf("Can't allocate resource context\n");
//         return (-1);
//     }
//
//     (*resource)->ptr = allocator->shared(size);
//     if((*resource)->ptr == NULL)
//     {
//         printf("Can't alloc resource pointer\n");
//         return (-1);
//     }
//
//     return (0);
// }

// static struct mem_pool_shared *create_fd_pool(char *path)
// {
//     int32_t rtrn = 0;
//     struct memory_block *m_blk = NULL;
//     struct mem_pool_shared *pool = NULL;
//     struct output_writter *output = NULL;
//     struct memory_allocator *allocator = NULL;
//     struct random_generator *random = NULL;
//
//     /* Just grab the console writter, default allocator, and random generator
//       in here so we don't break the old api but can use the new create_random_file() interface. */
//     output = get_console_writter();
//     if(output == NULL)
//     {
//         printf("Failed to get console writter\n");
//         return (NULL);
//     }
//
//     allocator = get_default_allocator();
//     if(allocator == NULL)
//     {
//         output->write(ERROR, "Failed to get default memory allocator\n");
//         return (NULL);
//     }
//
//     random = get_default_random_generator(allocator, output);
//     if(random == NULL)
//     {
//         output->write(ERROR, "Failed to get random number generator\n");
//         return (NULL);
//     }
//
//     pool = mem_create_shared_pool(sizeof(int32_t *), POOL_SIZE);
//     if(pool == NULL)
//     {
//         printf("Can't allocate descriptor memory pool\n");
//         return (NULL);
//     }
//
//     /* Create a bunch of file descriptors and stick them into the resource pool. */
//     init_shared_pool(&pool, m_blk)
//     {
//         uint64_t size = 0;
//
//         /* Don't free that will be taken cared off later. */
//         struct resource_ctx *resource = NULL;
//
//         /* Declare a auto free pointer for storing the filepath. */
//         char *file_path auto_free = NULL;
//
//         /* Create a resource context. */
//         rtrn = init_resource_ctx(&resource, sizeof(int32_t));
//         if(rtrn < 0)
//         {
//             printf("Can't initialize resource context\n");
//             return (NULL);
//         }
//
//         /* Create a random file in temp. */
//         rtrn = create_random_file(path, ".txt", &file_path, &size, random, allocator, output);
//         if(rtrn < 0)
//         {
//             printf("Can't create random file\n");
//             return (NULL);
//         }
//
//         /* Open newly created file.  */
//         int32_t fd = open(file_path, O_RDWR, 0777);
//         if(fd < 0)
//         {
//             printf("Can't open newly created file: %s\n",
//                    strerror(errno));
//             return (NULL);
//         }
//
//         /* Move fd to shared memory. */
//         memmove(resource->ptr, &fd, sizeof(int32_t));
//
//         resource->m_blk = m_blk;
//
//         m_blk->ptr = resource;
//     }
//
//     return (pool);
// }
//
// static struct mem_pool_shared *create_file_pool(char *path)
// {
//     int32_t rtrn = 0;
//     struct memory_block *m_blk = NULL;
//     struct mem_pool_shared *pool = NULL;
//     struct output_writter *output = NULL;
//     struct memory_allocator *allocator = NULL;
//     struct random_generator *random = NULL;
//
//     /* Just grab the console writter, default allocator, and random generator
//       in here so we don't break the old api but can use the new create_random_file() interface. */
//     output = get_console_writter();
//     if(output == NULL)
//     {
//         printf("Failed to get console writter\n");
//         return (NULL);
//     }
//
//     allocator = get_default_allocator();
//     if(allocator == NULL)
//     {
//         output->write(ERROR, "Failed to get default memory allocator\n");
//         return (NULL);
//     }
//
//     random = get_default_random_generator(allocator, output);
//     if(random == NULL)
//     {
//         output->write(ERROR, "Failed to get random number generator\n");
//         return (NULL);
//     }
//
//     /* Create shared memory pool. */
//     pool = mem_create_shared_pool(sizeof(char *), POOL_SIZE);
//     if(pool == NULL)
//     {
//         printf("Can't allocate file path memory pool\n");
//         return (NULL);
//     }
//
//     /* Initialize shared pool with file paths. */
//     init_shared_pool(&pool, m_blk)
//     {
//         uint64_t file_size = 0;
//
//         /* Don't free, that will be taken cared off later. */
//         char *file_path = NULL;
//         struct resource_ctx *resource = NULL;
//
//         /* Initialize resource context. */
//         rtrn = init_resource_ctx(&resource, PATH_MAX + 1);
//         if(rtrn < 0)
//         {
//             printf("Initialize resource context\n");
//             return (NULL);
//         }
//
//         /* Create random file in /tmp. */
//         rtrn = create_random_file(path, ".txt", &file_path, &file_size, random, allocator, output);
//         if(rtrn < 0)
//         {
//             printf("Can't create random file\n");
//             return (NULL);
//         }
//
//         /* Move file path to shared memory. */
//         memmove(resource->ptr, file_path, strlen(file_path));
//
//         /* Set memory block pointer. */
//         resource->m_blk = m_blk;
//
//         /* Set resource pointer. */
//         m_blk->ptr = resource;
//     }
//
//     return (pool);
// }

/*static int32_t create_mount_pool(char *path)
{
	mount_pool = mem_create_shared_pool(sizeof(char *), POOL_SIZE);
	if(mount_pool == NULL)
	{
		printf("Can't allocate mount path memory pool\n");
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
            printf("Can't generate dir name\n");
            return (-1);
        }

        char *name = NULL;

        name = mem_alloc(9);
        if(name == NULL)
        {
            printf("Can't alloc name buffer\n");
            return (-1);
        }

        memcpy(name, fs_name, 8);

        printf("name: %s\n", name);

        rtrn = mount(name, path, MNT_FORCE, NULL);
        if(rtrn < 0)
        {
           printf("mount: %s\n", strerror(errno));
           return (-1);
        }

        block->ptr = name;
    }

	return (0);
} */

// static struct mem_pool_shared *create_dirpath_pool(char *path)
// {
//     struct mem_pool_shared *pool = NULL;
//     struct memory_allocator *allocator = NULL;
//     struct output_writter *output = NULL;
//
//     output = get_console_writter();
//     if(output == NULL)
//     {
//         printf("Failed to get console writter\n");
//         return (NULL);
//     }
//
//     pool = mem_create_shared_pool(sizeof(char *), POOL_SIZE);
//     if(pool == NULL)
//     {
//         printf("Can't allocate dir path memory pool\n");
//         return (NULL);
//     }
//
//     allocator = get_default_allocator();
//     if(allocator == NULL)
//     {
//         printf("Failed to get default memory allocator\n");
//         return (NULL);
//     }
//
//     int32_t rtrn = 0;
//     struct memory_block *m_blk = NULL;
//
//     NX_SLIST_FOREACH(m_blk, &pool->free_list)
//     {
//         /* Temp variable that we define with auto_clean so that we
//         don't have to worry about calling free. */
//         char *dir_name auto_free = NULL;
//
//         /* Don't free, that will be taken cared of later. */
//         char *dir_path = NULL;
//         struct resource_ctx *resource = NULL;
//
//         resource = allocator->shared(sizeof(struct resource_ctx));
//         if(resource == NULL)
//         {
//             printf("Can't alloc resource object\n");
//             return (NULL);
//         }
//
//         resource->ptr = allocator->shared(PATH_MAX + 1);
//         if(resource->ptr == NULL)
//         {
//             printf("Can't alloc resource pointer\n");
//             return (NULL);
//         }
//
//         /* Use generate_name to create a random directory name.  */
//         rtrn = generate_directory_name((char **)&dir_name, output);
//         if(rtrn < 0)
//         {
//             printf("Can't generate file path\n");
//             return (NULL);
//         }
//
//         /* Join the dir name with the temp path. */
//         rtrn = asprintf(&dir_path, "%s/%s", path, dir_name);
//         if(rtrn < 0)
//         {
//             printf("Can't join paths: %s\n", strerror(errno));
//             return (NULL);
//         }
//
//         memmove(resource->ptr, dir_path, strlen(dir_path));
//
//         resource->m_blk = m_blk;
//
//         m_blk->ptr = resource;
//     }
//
//     return (pool);
// }

// static int32_t clean_allocated_file_list(struct mem_pool_shared *pool)
// {
//     int32_t rtrn = 0;
//
//     struct memory_block *m_blk = NULL;
//     struct memory_allocator *allocator = NULL;
//
//     allocator = get_default_allocator();
//     if(allocator == NULL)
//     {
//         printf("Failed to get default memory allocator\n");
//         return (-1);
//     }
//
//     /* Loop and grab all the file paths in the allocated list. */
//     NX_SLIST_FOREACH(m_blk, &pool->allocated_list)
//     {
//         struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;
//
//         rtrn = unlink((char *)resource->ptr);
//         if(rtrn < 0)
//         {
//             printf("Can't remove temp file\n");
//             return (-1);
//         }
//
//         allocator->free_shared((void **)&resource->ptr, PATH_MAX + 1);
//         allocator->free_shared((void **)&m_blk->ptr, sizeof(struct resource_ctx));
//         allocator->free_shared((void **)&m_blk, sizeof(struct memory_block));
//     }
//
//     return (0);
// }

/*static int32_t clean_free_file_list(struct mem_pool_shared *pool)
{
    int32_t rtrn = 0;

    struct memory_block *m_blk = NULL;

    printf("pool: %p\n", (void *)pool);
    printf("free_list: %p\n", (void *)&pool->free_list);
    printf("allocated_list: %p\n", (void *)&pool->allocated_list);

    NX_SLIST_FOREACH(m_blk, &pool->free_list)
    {
        struct resource_ctx *resource = (struct resource_ctx *)m_blk->ptr;

        rtrn = unlink((char *)resource->ptr);
        if(rtrn < 0)
        {
            printf("Can't remove temp file\n");
            return (-1);
        }

        mem_free_shared((void **)&resource->ptr, PATH_MAX + 1);
        mem_free_shared((void **)&m_blk->ptr, sizeof(struct resource_ctx));
        mem_free_shared((void **)&m_blk, sizeof(struct memory_block));
    }

    return (0);
} */

// static int32_t clean_file_pool(struct mem_pool_shared *pool)
// {
//     int32_t rtrn = 0;
//
//     if(pool == NULL)
//     {
//         printf("File pool already clean\n");
//         return (-1);
//     }
//
//     /* Lock the spinlock. */
//     nx_spinlock_lock(&pool->lock);
//
//     if(CK_SLIST_EMPTY(&pool->allocated_list) != TRUE)
//     {
//         rtrn = clean_allocated_file_list(pool);
//         if(rtrn < 0)
//         {
//             printf("Can't clean allocated file list\n");
//             return (-1);
//         }
//     }
//
//     /*if(CK_SLIST_EMPTY(&pool->free_list) != TRUE)
//     {
//         rtrn = clean_free_file_list(pool);
//         if(rtrn < 0)
//         {
//             printf("Can't clean free file list\n");
//             return (-1);
//         }
//     } */
//
//     nx_spinlock_unlock(&pool->lock);
//
//     mem_clean_shared_pool(pool);
//
//     return (0);
// }
//
// static struct mem_pool_shared *create_socket_pool(void)
// {
//     struct mem_pool_shared *pool = NULL;
//
//     pool = mem_create_shared_pool(sizeof(int32_t), POOL_SIZE);
//     if(pool == NULL)
//     {
//         printf("Can't allocate socket memory pool\n");
//         return (NULL);
//     }
//
//     struct memory_allocator *allocator = NULL;
//
//     allocator = get_default_allocator();
//     if(allocator == NULL)
//     {
//         printf("Failed to get default memory allocator\n");
//         return (NULL);
//     }
//
//     int32_t rtrn = 0;
//     struct memory_block *m_blk = NULL;
//
//     NX_SLIST_FOREACH(m_blk, &pool->free_list)
//     {
//         int32_t *sock = NULL;
//
//         struct resource_ctx *resource = NULL;
//
//         /* Initialize resource context. */
//         rtrn = init_resource_ctx(&resource, sizeof(int32_t));
//         if(rtrn < 0)
//         {
//             printf("Initialize resource context\n");
//             return (NULL);
//         }
//
//         sock = allocator->alloc(sizeof(int32_t));
//         if(sock == NULL)
//         {
//             printf("Can't allocate socket\n");
//             return (NULL);
//         }
//
//         rtrn = connect_ipv6(sock);
//         if(rtrn < 0)
//         {
//             printf("Can't create socket\n");
//             allocator->free((void **)&sock);
//             return (NULL);
//         }
//
//         memmove(resource->ptr, sock, sizeof(int32_t));
//
//         resource->m_blk = m_blk;
//
//         m_blk->ptr = resource;
//     }
//
//     return (pool);
// }

struct resource_generator *get_resource_generator(struct memory_allocator *allocator,
                                                  struct output_writter *output)
{
    struct resource_generator *rsrc_gen = NULL;

    rsrc_gen = allocator->alloc(sizeof(struct resource_generator));
    if(rsrc_gen == NULL)
    {
        output->write(ERROR, "Failed to allocate resource generator\n");
        return (NULL);
    }

    rsrc_gen->get_desc = &get_desc_nocached;
    rsrc_gen->free_desc = &free_desc_nocached;

    return (rsrc_gen);
}

struct desc_generator *get_default_desc_generator(struct memory_allocator *allocator,
                                                  struct output_writter *output)
{
    struct desc_generator *desc_gen = allocator->alloc(sizeof(struct desc_generator));
    if(desc_gen == NULL)
    {
        output->write(ERROR, "Failed to allocate memory for the descriptor generator\n");
        return (NULL);
    }

    desc_gen->get_desc = &get_desc_nocached;
    desc_gen->free_desc = &free_desc_nocached;

    return (desc_gen);
}

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

#include "generate.h"
#include "crypto/random.h"
#include "io/io.h"
#include "memory/memory.h"
#include "network/network.h"
#include "resource/resource.h"
#include "runtime/nextgen.h"
#include "utils/utils.h"

#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

static struct output_writter *output;
static struct memory_allocator *allocator;
static struct random_generator *random_gen;
static struct resource_generator *rsrc_gen;

void inject_generate_deps(struct dependency_context *ctx)
{
    uint32_t i;

    for(i = 0; i < ctx->count; i++)
    {
        switch((int32_t)ctx->array[i]->name)
        {
            case ALLOCATOR:
                allocator = (struct memory_allocator *)ctx->array[i]->interface;
                break;

            case OUTPUT:
                output = (struct output_writter *)ctx->array[i]->interface;
                break;

            case RANDOM_GEN:
                random_gen = (struct random_generator *)ctx->array[i]->interface;
                break;

            case RESOURCE_GEN:
                rsrc_gen = (struct resource_generator *)ctx->array[i]->interface;
                break;
        }
    }
}

int32_t generate_mach_port(uint64_t **ptr)
{
  (void)ptr;


    return (0);
}

int32_t generate_ptr(uint64_t **ptr)
{
  int32_t rtrn = 0;
  uint32_t number = 0;
  uint32_t nbytes = 0;

  rtrn = random_gen->range(1023, &nbytes);
  if(rtrn < 0)
  {
      output->write(ERROR, "Can't pick random size\n");
      return (-1);
  }

  /* Add one to nbytes so that we don't have a zero.
  Which would cause mmap to fail on some platforms. */
  nbytes = nbytes + 1;

  rtrn = random_gen->range(2, &number);
  if(rtrn < 0)
  {
      output->write(ERROR, "Can't pick random number\n");
      return (-1);
  }

  switch(number)
  {
      case 0:
          (*ptr) = mmap(NULL, nbytes, PROT_READ | PROT_WRITE,
                        MAP_ANON | MAP_PRIVATE, -1, 0);
          if(*ptr == MAP_FAILED)
          {
              output->write(ERROR, "mmap: %s\n", strerror(errno));
              return (-1);
          }
          break;

      case 1:
          (*ptr) =
              mmap(NULL, nbytes, PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
          if(*ptr == MAP_FAILED)
          {
              output->write(ERROR, "mmap: %s\n", strerror(errno));
              return (-1);
          }
          break;

      case 2:
          (*ptr) =
              mmap(NULL, nbytes, PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
          if(*ptr == MAP_FAILED)
          {
              output->write(ERROR, "mmap: %s\n", strerror(errno));
              return (-1);
          }
          break;

      default:
          output->write(ERROR, "Should not get here\n");
          return (-1);
  }

    return (0);
}

int32_t generate_fd(uint64_t **fd)
{
    /* Allocate the descriptor. */
    (*fd) = allocator->alloc(sizeof(int32_t));
    if((*fd) == NULL)
    {
        output->write(ERROR, "Can't allocate a descriptor\n");
        return (-1);
    }

    /* Get a file descriptor from the descriptor pool. */
    int32_t desc = rsrc_gen->get_desc();
    if(desc < 0)
    {
        output->write(ERROR, "Can't get file descriptor\n");
        return (-1);
    }

    memmove((*fd), &desc, sizeof(int32_t));

    return (0);
}

int32_t generate_socket(uint64_t **sock)
{
    (*sock) = allocator->alloc(sizeof(int32_t));
    if((*sock) == NULL)
    {
        output->write(ERROR, "Can't allocate socket\n");
        return (-1);
    }

    int32_t sock_fd = 0;

    sock_fd = rsrc_gen->get_socket();
    if(sock_fd < 0)
    {
        output->write(ERROR, "Can't get socket\n");
        return (-1);
    }

    memmove((*sock), &sock_fd, sizeof(int32_t));

    return (0);
}

int32_t generate_buf(uint64_t **buf)
{
    int32_t rtrn = 0;
    uint32_t number = 0;
    uint32_t nbytes = 0;

    rtrn = random_gen->range(1023, &nbytes);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't pick random size\n");
        return (-1);
    }

    /* Add one to nbytes so that we don't have a zero.
    Which would cause mmap to fail on some platforms. */
    nbytes = nbytes + 1;

    rtrn = random_gen->range(2, &number);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't pick random number\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            (*buf) = mmap(NULL, nbytes, PROT_READ | PROT_WRITE,
                          MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output->write(ERROR, "mmap: %s\n", strerror(errno));
                return (-1);
            }
            break;

        case 1:
            (*buf) =
                mmap(NULL, nbytes, PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output->write(ERROR, "mmap: %s\n", strerror(errno));
                return (-1);
            }
            break;

        case 2:
            (*buf) =
                mmap(NULL, nbytes, PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output->write(ERROR, "mmap: %s\n", strerror(errno));
                return (-1);
            }
            break;

        default:
            output->write(ERROR, "Should not get here\n");
            return (-1);
    }

    return (0);
}

int32_t generate_path(uint64_t **path)
{
    (*path) = (uint64_t *)rsrc_gen->get_filepath();
    if((*path) == NULL)
    {
        output->write(ERROR, "Can't get file path\n");
        return (-1);
    }

    return (0);
}

int32_t generate_int(uint64_t **num)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    rtrn = random_gen->range(INT32_MAX, &number);
    if(rtrn < 0)
    {
        output->write(ERROR, "Failed to generate random number\n");
        return (-1);
    }

    (*num) = allocator->alloc(sizeof(uint64_t));
    if((*num) == NULL)
    {
        output->write(ERROR, "Failed to allocate buffer\n");
        return (-1);
    }

    (**num) = number;

    return (0);
}

int32_t generate_pid(uint64_t **pid)
{
    pid_t id = 0;

    (*pid) = allocator->alloc(sizeof(uint64_t));
    if((*pid) == NULL)
    {
        output->write(ERROR, "Failed to allocate buffer\n");
        return (-1);
    }

    id = fork();
    if(id == 0)
    {
        while(1)
        {
            usleep(1000);
        }
    }
    else if(id > 0)
    {
        (**pid) = (uint64_t) id;

        return (0);
    }
    else
    {
        output->write(ERROR, "Failed to create child process: %s\n", strerror(errno));
        return (-1);
    }
}

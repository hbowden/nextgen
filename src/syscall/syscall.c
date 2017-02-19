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

#include "syscall.h"
#include "arg_types.h"
#include "child.h"
#include "signals.h"
#include "generate.h"
#include "crypto/random.h"
#include "memory/memory.h"
#include "resource/resource.h"
#include "runtime/platform.h"
#include "io/io.h"

#include <signal.h>
#include <errno.h>

static struct resource_generator *rsrc_gen;
static struct memory_allocator *allocator;
static struct output_writter *output;
static struct random_generator *random_gen;

struct test_case
{
    uint64_t **arg_value_array;
    struct syscall_entry *entry;
};

struct test_case *create_test_case(void)
{
    struct test_case *test = NULL;

    test = allocator->alloc(sizeof(struct test_case));
    if(test == NULL)
    {
        output->write(ERROR, "Failed to allocate test case\n");
        return (NULL);
    }

    test->arg_value_array = allocator->alloc(sizeof(uint64_t *) * ARG_LIMIT);
    if(test->arg_value_array == NULL)
    {
        output->write(ERROR, "Allocation failed\n");
        return (NULL);
    }

    test->entry = pick_syscall(get_table());
    if(test->entry == NULL)
    {
        output->write(ERROR, "Can't pick syscall\n");
        return (NULL);
    }

    int32_t rtrn = generate_args(test);
    if(rtrn < 0)
    {
        output->write(ERROR, "Failed to generate syscall arguments\n");
        return (NULL);
    }

    return (test);
}

struct test_case *create_test_case_for(const char *name)
{
    struct test_case *test = NULL;

    test = allocator->alloc(sizeof(struct test_case));
    if(test == NULL)
    {
        output->write(ERROR, "Failed to allocate test case\n");
        return (NULL);
    }

    test->arg_value_array = allocator->alloc(sizeof(uint64_t *) * ARG_LIMIT);
    if(test->arg_value_array == NULL)
    {
        output->write(ERROR, "Allocation failed\n");
        return (NULL);
    }

    test->entry = find_entry(name, get_table());
    if(test->entry == NULL)
    {
        output->write(ERROR, "Can't find syscall\n");
        return (NULL);
    }

    int32_t rtrn = generate_args(test);
    if(rtrn < 0)
    {
        output->write(ERROR, "Failed to generate syscall arguments\n");
        return (NULL);
    }

    return (test);
}

void cleanup_test(struct test_case *test)
{
    uint32_t i;
    int32_t rtrn = 0;
    uint32_t total_args = test->entry->total_args;

    for(i = 0; i < total_args; i++)
    {
        /* Handle args that require special cleanup procedures. */
        switch((int32_t)test->entry->arg_type_array[i])
        {
            /* Below is the resource types ie they are from the resource module.
            They must be freed using special functions and the free must be done on
            the arg_copy_index so the free_* functions don't use the mutated value in arg_value_index. */
            case FILE_DESC:
                rtrn = rsrc_gen->free_desc((int32_t *)test->arg_value_array[i]);
                if(rtrn < 0)
                    output->write(ERROR, "Can't free descriptor\n");
                /* Don't return on errors, just keep looping. */
                break;

            case FILE_PATH:
                rtrn = rsrc_gen->free_filepath((char **)&(test->arg_value_array[i]));
                if(rtrn < 0)
                    output->write(ERROR, "Can't free filepath\n");
                /* Don't return on errors, just keep looping. */
                break;

            case DIR_PATH:
                rtrn = rsrc_gen->free_dirpath((char **)&(test->arg_value_array[i]));
                if(rtrn < 0)
                    output->write(ERROR, "Can't free dirpath\n");
                /* Don't return on errors, just keep looping. */
                break;

            case SOCKET:
                rtrn = rsrc_gen->free_socket((int32_t *)test->arg_value_array[i]);
                if(rtrn < 0)
                    output->write(ERROR, "Can't free socket\n");
                /* Don't return on errors, just keep looping. */
                break;
            /* End of resource types. */

            /* Kill the temp process using the copy value
               so that we don't use the mutated value
               in arg_value_index[i]. */
            case PID:
                rtrn = kill((pid_t)(*test->arg_value_array[i]), SIGKILL);
                if(rtrn < 0)
                    output->write(ERROR, "Can't kill child: %s\n", strerror(errno));
                    /* Don't return on errors, just keep looping. */
                break;

            case ADDRESS:
                // allocator->free_shared((void **)&test->arg_value_array[i]);
                break;

            default:
                /* Free value if non NULL. */
                //mem_free((void **)&ctx->arg_value_array[i]);
                break;
        }

        // allocator->free((void **)&test->arg_value_array[i]);
    }

    allocator->free((void **)&test);
}

 
inline uint64_t **get_argument_array(struct test_case *test)
{
    return (test->arg_value_array);
}

inline uint32_t get_total_args(struct test_case *test)
{
    return (test->entry->total_args);
}

inline struct syscall_entry *get_entry(struct test_case *test)
{
    return (test->entry);
}

struct syscall_entry *pick_syscall(struct syscall_table *table)
{
    uint32_t offset = 0;

    int32_t rtrn = random_gen->range(table->total_syscalls, &offset);
    if(rtrn < 0)
    {
        output->write(ERROR, "Failed to calculate offset\n");
        return (NULL);
    }

    return (atomic_load_ptr(&table->sys_entry[offset]));
}

int32_t generate_args(struct test_case *test)
{
    uint32_t i;
    int32_t rtrn = 0;
    struct syscall_entry *entry = test->entry;
    uint32_t total_args = test->entry->total_args;

    for(i = 0; i < total_args; i++)
    {
        rtrn = entry->get_arg_array[i](&test->arg_value_array[i]);
        if(rtrn < 0)
        {
            output->write(ERROR, "Failed to generate syscall argument\n");
            return (-1);
        }
    }

    return (0);
}

struct syscall_entry *find_entry(const char *name, struct syscall_table *table)
{
    uint32_t i;
    struct syscall_entry *entry = NULL;
    uint32_t total_syscalls = table->total_syscalls;

    for(i = 0; i < total_syscalls; i++)
    {
        if(strcmp(name, table->sys_entry[i]->syscall_name) == 0)
        {
            entry = table->sys_entry[i];
            break;
        }
    }

    return (entry);
}

void inject_syscall_deps(struct dependency_context *ctx)
{
    inject_arg_types_deps(ctx);
    inject_child_deps(ctx);
    inject_signal_deps(ctx);
    inject_generate_deps(ctx);

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
        }
    }

    return;
}

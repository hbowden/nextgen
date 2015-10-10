

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

#include "nextgen.h"
#include "crypto.h"
#include "io.h"
#include "memory.h"
#include "file.h"
#include "reaper.h"
#include "plugin.h"
#include "resource.h"
#include "runtime.h"

#include <sys/param.h>
#include <sys/mount.h>
#include <ck_queue.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <dtrace.h>
#include <sys/mman.h>

struct shared_map *map;

struct parser_ctx
{
    enum fuzz_mode mode;
    char *path_to_exec;
    char *path_to_in_dir;
    char *path_to_out_dir;
    char *crypto_method;
    char *args;
    int crypto_flag;
    int smart_mode;
};

static const char *optstring = "p:e:";

static struct option longopts[] = {

    { "in", required_argument, NULL, 'i' },
    { "out", required_argument, NULL, 'o' },
    { "exec", required_argument, NULL, 'e'},
    { "crypto", required_argument, NULL, 'c' },
    { "port", required_argument, NULL, 'p'},
    { "address", required_argument, NULL, 'a'},
    { "protocol", required_argument, NULL, 'c'},
    { "args", required_argument, NULL, 'x'},
    { "file", 0, NULL, 'f'},
    { "network", 0, NULL, 'n'},
    { "syscall", 0, NULL, 's'},
    { "help", 0, NULL, 'h'},
    { "dumb", 0, NULL, 'd'},
    { NULL, 0, NULL, 0 }
};

static void display_help_banner(void)
{
    output(STD, "Nextgen is a Genetic File, Syscall, and Network Fuzzer.\n");
    output(STD, "To use the file fuzzer in smart mode run the command below.\n");
    output(STD, "sudo ./nextgen --file --in /path/to/in/directory --out /path/to/out/directory --exec /path/to/target/exec .\n");

    output(STD, "To use the syscall fuzzer in smart mode run.\n");
    output(STD, "sudo ./nextgen --syscall --out /path/to/out/directory\n");

    output(STD, "To use dumb mode just pass --dumb with any of the above commands.\n");

    return;
}

static struct parser_ctx *parse_cmd_line(int argc, char *argv[])
{
    int ch, rtrn;
    int iFlag = FALSE, oFlag = FALSE, fFlag = FALSE, nFlag = FALSE, 
    sFlag = FALSE, eFlag = FALSE, pFlag = FALSE, aFlag = FALSE, tFlag = FALSE;

    struct parser_ctx *ctx = mem_alloc(sizeof(struct parser_ctx));
    if(ctx == NULL)
    {
        output(ERROR, "Can't allocate parser context\n");
        return NULL;
    }

    /* Default to smart_mode. */
    ctx->smart_mode = TRUE;

    while((ch = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
    {
        switch(ch)
        {
            /* Display banner and exit. */
            case 'h':
                display_help_banner();
                return NULL;

            case 'p':
                pFlag = TRUE;
                break;

            case 'a':
                aFlag = TRUE;
                break;

            case 't':
                tFlag = TRUE;
                break;

            case 'e':
                rtrn = asprintf(&ctx->path_to_exec, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return NULL;
                }

                eFlag = TRUE;
                break;

            case 'i':
                rtrn = asprintf(&ctx->path_to_in_dir, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return NULL;
                }

                iFlag = TRUE;
                break;

            case 'o':
                rtrn = asprintf(&ctx->path_to_out_dir, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return NULL;
                }

                oFlag = TRUE;
                break;

            case 'f':
                fFlag = TRUE;
                ctx->mode = MODE_FILE;
                break;

            case 'n':
                nFlag = TRUE;
                ctx->mode = MODE_NETWORK;
                break;

            case 's':
                sFlag = TRUE;
                ctx->mode = MODE_SYSCALL;
                break;

            case 'c':
                /* This option allows users to specify the method in which they want to derive
                the random numbers that will be used in fuzzing the application. */
                rtrn = asprintf(&ctx->crypto_method, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return NULL;
                }
                
                ctx->crypto_flag = TRUE;

                break;

            case 'd':

                ctx->smart_mode = FALSE;

                break;

            case 'x':
                rtrn = asprintf(&ctx->args, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return NULL;
                }
                break;

            default:
              output(ERROR, "Unknown option\n");
              return NULL;
        }
    }

    /* Make sure a fuzzing mode was selected. */
    if(fFlag != TRUE && nFlag != TRUE && sFlag != TRUE)
    {
        output(STD, "Specify a fuzzing mode\n");
        return NULL;
    }

    /* If file mode was selected lets make sure all the right args were passed.*/
    if(fFlag == TRUE)
    {
        if(iFlag == FALSE || oFlag == FALSE || eFlag == FALSE)
        {
            output(STD, "Pass --exec , --in and --out for file mode\n");
            return NULL;
        }
    }

    if(nFlag == TRUE)
    {
        if(aFlag == FALSE || tFlag == FALSE || oFlag == FALSE)
        {
            output(STD, "Pass --address , --port, --protocol, and --out for network mode\n");
            return NULL;
        }
    }

    /* Check to see if syscall mode was selected. */
    if(sFlag == TRUE)
    {
        rtrn = check_root();
        if(rtrn != 0)
        {
            output(STD, "Run nextgen as root\n");
            return NULL;
        }

        /* Make sure the user set an output path. */
        if(oFlag != TRUE)
        {
            output(STD, "Pass --out for syscall mode\n");
            return NULL;
        }
    }

    return ctx;
}

static void clean_syscall_mapping(void)
{
    unsigned int i, ii;

    mem_free_shared(map->sys_table, sizeof(struct syscall_table));

    clean_resource_pools();

    for(i = 0; i < map->number_of_children; i++)
    {
        struct child_ctx *child = map->children[i];

        clean_shared_pool(child->pool);

        for(ii = 0; ii < 6; ii++)
        {
            mem_free_shared(child->arg_value_index[ii], sizeof(unsigned long));
        }

        mem_free(child->arg_value_index);

        mem_free(child->arg_size_index);
    
        mem_free(child->err_value);

        mem_free_shared(child, sizeof(struct child_ctx));
    }

    return;
}

static void clean_shared_mapping(void)
{
    /* Do mode specific shared mapping cleanup. */
    switch((int)map->mode)
    {
        case MODE_SYSCALL:
            clean_syscall_mapping();
            break;

        case MODE_FILE:
           
            break;

        case MODE_NETWORK:
           
            break;

        default:
            output(ERROR, "Unknown fuzzing mode\n");
            return;
    }
    return;
}

static int init_file_mapping(struct shared_map **mapping, struct parser_ctx *ctx)
{
    int rtrn;
    unsigned int i;

    /* Set the input and output directories. */
    (*mapping)->path_to_out_dir = ctx->path_to_out_dir;
    (*mapping)->path_to_in_dir = ctx->path_to_in_dir;

    unsigned int count = 0;

    /* Allocate the executable context object. */
    (*mapping)->exec_ctx = mem_alloc_shared(sizeof(struct executable_context));
    if((*mapping)->exec_ctx == NULL)
    {
        output(ERROR, "Can't create shared object\n");
        return -1;
    }

    /* Set exec path. */
    (*mapping)->exec_ctx->path_to_exec = ctx->path_to_exec;

    /* Count how many files are in the input directory. */
    rtrn = count_files_directory(&count);
    if(rtrn < 0)
    {
        output(ERROR, "Can't count files in the in directory.\n");
        return -1;
    }

    /* Set file_count in map so we know this value later. */
    (*mapping)->file_count = count;

    /* Create file index. */
    (*mapping)->file_index = mem_alloc((count + 1) * sizeof(char *));
    if((*mapping)->file_index == NULL)
    {
        output(ERROR, "Can't create file index: %s\n", strerror(errno));
        return -1;
    }

    for(i = 0; i < count; i++)
    {
        (*mapping)->file_index[i] = mem_alloc_shared(1025);
        if((*mapping)->file_index[i] == NULL)
        {
            output(ERROR, "Can't create shared object\n");
            return -1;
        }
    }

    /* Count plugins in the plugin directory. */
    rtrn = count_plugins(&(*mapping)->plugin_count);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create shared object\n");
        return -1;
    }

    /* Create plugin context structure index. */
    (*mapping)->plugins = mem_alloc(((*mapping)->plugin_count + 1) * sizeof(struct plugin_ctx *));
    if((*mapping)->plugins == NULL)
    {
        output(ERROR, "Can't create plugin index: %s\n", strerror(errno));
        return -1;
    }

    /* Loop and create each plugin context struct. */
    for(i = 0; i < (*mapping)->plugin_count; i++)
    {
        struct plugin_ctx *p_ctx = NULL;

        /* Map the plugin ctx struct as shared memory.*/
        p_ctx = mem_alloc_shared(sizeof(struct plugin_ctx));
        if(p_ctx == NULL)
        {
            output(ERROR, "Can't create shared plugin context\n");
            return -1;
        }

        (*mapping)->plugins[i] = p_ctx;
    }

    return 0;
}

static int init_network_mapping(struct shared_map **mapping, struct parser_ctx *ctx)
{

    return 0;
}

static int init_syscall_mapping(struct shared_map **mapping, struct parser_ctx *ctx)
{
    int rtrn;
    unsigned int i, ii;

    (*mapping)->path_to_out_dir = ctx->path_to_out_dir;

    /* Set running children to zero. */
    atomic_init(&(*mapping)->running_children, 0);

    /* We use atomic values for the pids, so let's init the reaper pid. */
    atomic_init(&(*mapping)->reaper_pid, 0);
    atomic_init(&(*mapping)->god_pid, 0);

    atomic_init(&(*mapping)->test_counter, 0);

    /* Allocate the system call table as shared memory. */
    (*mapping)->sys_table = mem_alloc_shared(sizeof(struct syscall_table));
    if((*mapping)->sys_table == NULL)
    {
        output(ERROR, "Can't create shared object\n");
        return -1;
    }

    /* Intialize socket server values.*/
    (*mapping)->socket_server_port = 0;
    atomic_init(&(*mapping)->socket_server_pid, 0);

    /* Grab the core count of the machine we are on. */
    unsigned int core_count;
    rtrn = get_core_count(&core_count);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get core count\n");
        return -1;
    }
    
    /* Set the number of child processes to the number of cores detected. */
    (*mapping)->number_of_children = core_count;

    /* Create the child process structures. */
    (*mapping)->children = mem_alloc((*mapping)->number_of_children * sizeof(struct child_ctx *));
    if((*mapping)->children == NULL)
    {
        output(ERROR, "Can't create children object.\n");
        return -1;
    }

    /* Loop for each child and allocate the child context object as shared anonymous memory. */
    for(i = 0; i < (*mapping)->number_of_children; i++)
    {
        /* Init a new child context struct. */
        struct child_ctx child_struct = { .list = CK_LIST_HEAD_INITIALIZER(child->list) };
        struct child_ctx *child = &child_struct;

        /* Map the child context as shared memory. */
        child = mem_alloc_shared(sizeof(struct child_ctx));
        if(child == NULL)
        {
            output(ERROR, "Can't create child context\n");
            return -1;
        }

        child->current_arg = 0;

        /* Create a shared memory pool for the child. */
        child->pool = mem_create_shared_pool(sizeof(struct list_node), 4096);
        if(child->pool == NULL)
        {
            output(ERROR, "Can't create child's memory pool\n");
            return -1;
        }

        struct memory_block *block = NULL;

        init_shared_pool(&child->pool, block)
        {
            struct list_node *node = NULL;

            node = mem_alloc_shared(sizeof(struct list_node));
            if(node == NULL)
            {
                output(ERROR, "Can't alloc list node\n");
                return -1;
            }

            block->ptr = node;

            struct list_data *data = NULL;

            data = mem_alloc_shared(sizeof(struct list_data));
            if(data == NULL)
            {
                output(ERROR, "Can't alloc list data\n");
                return -1;
            }

            ((struct list_node *)block->ptr)->data = data;
        }
     
        /* Create the index where we store the syscall arguments. */
        child->arg_value_index = mem_alloc(7 * sizeof(unsigned long *));
        if(child->arg_value_index == NULL)
        {
            output(ERROR, "Can't create arg value index: %s\n", strerror(errno));
            return -1;
        }

        /* This index tracks the size of of the argument generated.  */
        child->arg_size_index = mem_alloc(7 * sizeof(unsigned long));
        if(child->arg_size_index == NULL)
        {
            output(ERROR, "Can't create arg size index: %s\n", strerror(errno));
            return -1;
        }

        /* This is where we store the error string on each syscall test. */
        child->err_value = mem_alloc(1024);
        if(child->err_value == NULL)
        {
            output(ERROR, "err_value: %s\n", strerror(errno));
            return -1;
        }

        /* Init cleanup list. */
        CK_LIST_INIT(&child->list);

        /* Loop and create the various indecies in the child struct. */
        for(ii = 0; ii < 6; ii++)
        {
            child->arg_value_index[ii] = mem_alloc_shared(sizeof(unsigned long));
            if(child->arg_value_index[ii] == NULL)
            {
                output(ERROR, "Can't create arg value\n");
                return -1;
            }
        }

        atomic_init(&child->pid, EMPTY);

        (*mapping)->children[i] = child;
    }

    return 0;
}

/**
 * We use this function to initialize all the shared maps member values.
 **/
static int init_shared_mapping(struct shared_map **mapping, struct parser_ctx *ctx)
{
    int rtrn;

    /* Set the fuzzing mode selected by the user. */
    (*mapping)->mode = ctx->mode;

    /* Set inteligence mode. */
    (*mapping)->smart_mode = ctx->smart_mode;

    /* Set the stop flag to FALSE, when set to TRUE all processes start their exit routines and eventually exit. */
    atomic_init(&(*mapping)->stop, FALSE);

    /* Init the runloop pid. */
    atomic_init(&(*mapping)->runloop_pid, 0);

    /* Do mode specific shared mapping setup. */
    switch((int)ctx->mode)
    {
        case MODE_SYSCALL:
            rtrn = init_syscall_mapping(mapping, ctx);
            if(rtrn < 0)
            {
                output(ERROR, "Can't init syscall mapping\n");
                return -1;
            }
            break;

        case MODE_FILE:
            rtrn = init_file_mapping(mapping, ctx);
            if(rtrn < 0)
            {
                output(ERROR, "Can't init file mapping\n");
                return -1;
            }
            break;

        case MODE_NETWORK:
            rtrn = init_network_mapping(mapping, ctx);
            if(rtrn < 0)
            {
                output(ERROR, "Can't init network mapping\n");
                return -1;
            }
            break;

        default:
            output(ERROR, "Unknown fuzzing mode\n");
            return -1;
    }
    
    return 0;

}

/**
 * Main is the entry point to nextgen. In main we check for root, unfortunetly we need root to execute.
 * This is because we have to use dtrace, as well as bypass sandboxes, inject code into processes and 
 * other activities that require root access. We then create shared memory so we can share information
 * between processes. Next we parse the command line for user arguments and we stick the results in the
 * shared memory map. After parsing we set up the enviroment to the user's specfications and then finnaly
 * start the runtime, ie start fuzzing.
 **/
int main(int argc, char *argv[])
{
    int rtrn;
    struct parser_ctx *ctx = NULL;

    /* Create a shared memory map so that we can share state with other threads and procceses. */
    map = mem_alloc_shared(sizeof(struct shared_map));
    if(map == NULL)
    {
        output(ERROR, "Can't create shared object.\n");
        return -1;
    }

    /* Parse the command line for user input. parse_cmd_line() will set variables
    in map, the shared memory object. This will tell the fuzzer how to run. */
    ctx = parse_cmd_line(argc, argv);
    if(ctx == NULL)
    {
        output(ERROR, "Can't parse command line.\n");
        return -1;
    }

    /* Setup the shared map now that we got our options from the command line. */
    rtrn = init_shared_mapping(&map, ctx);
    if(rtrn < 0)
    {
        output(ERROR, "Can't initialize.\n");
        return -1;
    }

    /* Setup the fuzzer running enviroment. */
    rtrn = setup_runtime();
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup runtime enviroment.\n");
        clean_shared_mapping();
        return -1;
    }

    /* Start the main fuzzing loop. */
    rtrn = start_runtime();
    if(rtrn < 0)
    {
        output(ERROR, "Can't start runtime enviroment.\n");
        clean_shared_mapping();
        return -1;
    }
    
    /* We should only reach here on ctrl-c. */
    clean_shared_mapping();
    return 0;
}

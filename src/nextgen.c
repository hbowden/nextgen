

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
#include "utils.h"
#include "runtime.h"

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

static int check_root(void)
{
    output(STD, "Making sure the fuzzer has root privileges\n");

    uid_t check;

    check = getuid();
    if(check == 0)
    {
        return 0;
    }

    return -1;
}

static struct parser_ctx *parse_cmd_line(int argc, char *argv[])
{
    int ch, rtrn;
    int iFlag = FALSE, oFlag = FALSE, fFlag = FALSE, nFlag = FALSE, 
    sFlag = FALSE, eFlag = FALSE, pFlag = FALSE, aFlag = FALSE, tFlag = FALSE;

    struct parser_ctx *ctx = malloc(sizeof(struct parser_ctx));
    if(ctx == NULL)
    {
        output(ERROR, "malloc: %s\n", strerror(errno));
        return NULL;
    }

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

    /* Check if we have root, if we don't have root make sure the user passed
    --dumb or -d to specify they want dumb mode. */
    rtrn = check_root();
    if(rtrn != 0)
    {
        if(map->smart_mode != FALSE)
        {
            output(STD, "Either run as root to use smart mode or pass --dumb so that you do not have run as root\n");
            return NULL;
        }
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
        /* Make sure the user set an output path. */
        if(oFlag != TRUE)
        {
            output(STD, "Pass --out for syscall mode\n");
            return NULL;
        }
    }

    return ctx;
}

/**
 * We use this function to initialize all the shared maps member values.
 **/
static int init_shared_mapping(struct shared_map **mapping, struct parser_ctx *ctx)
{
    /* Do work that is common to the different fuzz modes. Then
    start doing work specific to the different fuzz mode. */
    int rtrn;

    /* Set the mode selected by the user. */
    (*mapping)->mode = ctx->mode;

    /* Check if the user selected dumb mode. */
    if(ctx->smart_mode != TRUE)
    {
        (*mapping)->smart_mode = FALSE;
    }
    else
    {
        /* We default to smart_mode being on, however the user can switch to
        dumb mode by passing --dumb on the command line. */
        (*mapping)->smart_mode = TRUE;
    }

    /* Set running children to zero. */
    atomic_init(&(*mapping)->running_children, 0);

    /* Set the stop flag to FALSE, when set to TRUE all processes start their exit routines and eventually exit. */
    atomic_init(&(*mapping)->stop, FALSE);

    /* We use atomic values for the pids, so let's init them. */
    atomic_init(&(*mapping)->reaper_pid, 0);
    atomic_init(&(*mapping)->runloop_pid, 0);

    /* Check if the user selected syscall mode. */
    if((*mapping)->mode == MODE_SYSCALL)
    {
        /* Allocate the system call table as shared memory. */
        rtrn = create_shared((void **)&(*mapping)->sys_table, sizeof(struct syscall_table));
        if(rtrn < 0)
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
    }

    /* Check if the user selected file mode. */
    if((*mapping)->mode == MODE_FILE)
    {
        /* Allocate the executable context object. */
        rtrn = create_shared((void **)&(*mapping)->exec_ctx, sizeof(struct executable_context));
        if(rtrn < 0)
        {
            output(ERROR, "Can't create shared object\n");
            return -1;
        }
    }

    /* Create the child process structures. */
    (*mapping)->children = malloc((*mapping)->number_of_children * sizeof(struct child_ctx *));
    if((*mapping)->children == NULL)
    {
        output(ERROR, "Can't create children object.\n");
        return -1;
    }

    unsigned int i;
    unsigned int ii;
    unsigned int iii;

    /* Loop for each child and allocate the child context object as shared anonymous memory. */
    for(i = 0; i < (*mapping)->number_of_children; i++)
    {
        /* Init a new child context struct. */
        struct child_ctx child_struct = { .list = CK_LIST_HEAD_INITIALIZER(child ->list) };
        struct child_ctx *child = &child_struct;

        /* Map the child context as shared memory. */
        rtrn = create_shared((void **)&child, sizeof(struct child_ctx));
        if(rtrn < 0)
        {
            output(ERROR, "Can't create shared object\n");
            return -1;
        }

        rtrn = create_shared((void **)&child->pool, sizeof(struct node_memory_pool));
        if(rtrn < 0)
        {
            output(ERROR, "Can't create memory pool\n");
            return -1;
        }
       
        /* If syscall mode is on then do some more child setup work. */
        if((*mapping)->mode == MODE_SYSCALL)
        {
            /* Create the index where we store the syscall arguments. */
            child->arg_value_index = malloc(7 * sizeof(unsigned long *));
            if(child->arg_value_index == NULL)
            {
                output(ERROR, "Can't create arg value index: %s\n", strerror(errno));
                return -1;
            }

            /* This index tracks the size of of the argument generated.  */
            child->arg_size_index = malloc(7 * sizeof(unsigned long));
            if(child->arg_size_index == NULL)
            {
                output(ERROR, "Can't create arg size index: %s\n", strerror(errno));
                return -1;
            }

            /* This is where we store the error string on each syscall test. */
            child->err_value = malloc(1024);
            if(child->err_value == NULL)
            {
                output(ERROR, "err_value: %s\n", strerror(errno));
                return -1;
            }
        }

        /* Init cleanup list. */
        CK_LIST_INIT(&child->list);

        for(ii = 0; ii < 6; ii++)
        {
            rtrn = create_shared((void **)&child->arg_value_index[ii], sizeof(unsigned long));
            if(rtrn < 0)
            {
                output(ERROR, "Can't create arg value\n");
                return -1;
            }

            rtrn = create_shared((void **)&child->arg_size_index[ii], sizeof(unsigned long));
            if(rtrn < 0)
            {
                output(ERROR, "Can't create arg size\n");
                return -1;
            }
        }

        child->pool->node = malloc(9 * sizeof(struct list_node *));
        if(child->pool->node == NULL)
        {
            output(ERROR, "Can't create memory pool\n");
            return -1;
        }

        /* Create memory pool for list_nodes. */
        for(iii = 0; iii < 8; iii++)
        {
            rtrn = create_shared((void **)&child->pool->node[iii], sizeof(struct list_node));
            if(rtrn < 0)
            {
                output(ERROR, "Can't create shared object\n");
                return -1;
            }

            rtrn = create_shared((void **)&child->pool->node[iii]->data, sizeof(struct list_data));
            if(rtrn < 0)
            {
                output(ERROR, "Can't create shared object\n");
                return -1;
            }
        }

        /* Init the slot values. */
        atomic_init(&child->pool->slot1_full, FALSE);
        atomic_init(&child->pool->slot2_full, FALSE);
        atomic_init(&child->pool->slot3_full, FALSE);
        atomic_init(&child->pool->slot4_full, FALSE);
        atomic_init(&child->pool->slot5_full, FALSE);
        atomic_init(&child->pool->slot6_full, FALSE);
        atomic_init(&child->pool->slot7_full, FALSE);
        atomic_init(&child->pool->slot8_full, FALSE);

        atomic_init(&child->pid, EMPTY);

        (*mapping)->children[i] = child;
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
    rtrn = create_shared((void **)&map, sizeof(struct shared_map));
    if(rtrn < 0)
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

    /* Setup the program running enviroment. */
    rtrn = setup_runtime();
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup runtime enviroment.\n");
        return -1;
    }

    /* Start the main fuzzing loop. */
    rtrn = start_runtime();
    if(rtrn < 0)
    {
        output(ERROR, "Can't start runtime enviroment.\n");
        return -1;
    }
    
    /* We should only reach here on ctrl-c. */
    return 0;
}
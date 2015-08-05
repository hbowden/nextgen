

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
#include "utils.h"
#include "crypto.h"
#include "runtime.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <dtrace.h>
#include <sys/mman.h>

struct shared_map *map;

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
    output(STD, "To use the file fuzzer run the command below.\n");
    output(STD, "sudo ./nextgen --file --in /path/to/in/directory --out /path/to/out/directory --exec /path/to/target/exec .\n");
    return;
}

static int parse_cmd_line(int argc, char *argv[])
{
    int ch, rtrn;
    int iFlag = FALSE, oFlag = FALSE, fFlag = FALSE, nFlag = FALSE, 
    sFlag = FALSE, eFlag = FALSE, pFlag = FALSE, aFlag = FALSE, tFlag = FALSE;

    while((ch = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
    {
        switch(ch)
        {
            /* Display banner and exit. */
            case 'h':
                display_help_banner();
                return -1;

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
                rtrn = asprintf(&map->path_to_exec, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return -1;
                }

                eFlag = TRUE;
                break;

            case 'i':
                rtrn = asprintf(&map->path_to_in_dir, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return -1;
                }

                iFlag = TRUE;
                break;

            case 'o':
                rtrn = asprintf(&map->path_to_out_dir, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return -1;
                }

                oFlag = TRUE;
                break;

            case 'f':
                fFlag = TRUE;
                break;

            case 'n':
                nFlag = TRUE;
                map->mode = MODE_NETWORK;
                break;

            case 's':
                sFlag = TRUE;
                map->mode = MODE_SYSCALL;
                break;

            case 'c':
                /* This option allows users to specify the method in which they want to derive
                the random numbers that will be used in fuzzing the application. */
                rtrn = asprintf(&map->crypto_method, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return -1;
                }
                
                map->crypto_flag = TRUE;

                break;

            case 'd':

                map->dumb_mode = TRUE;

                break;

            default:
              output(ERROR, "Unknown option\n");
              return -1;
        }
    }

    /* Make sure a fuzzing mode was selected. */
    if(fFlag != TRUE && nFlag != TRUE && sFlag != TRUE)
    {
        output(STD, "Specify a fuzzing mode\n");
        return -1;
    }

    /* If file mode was selected lets make sure all the right args were passed.*/
    if(fFlag == TRUE)
    {
        if(iFlag == FALSE || oFlag == FALSE || eFlag == FALSE)
        {
            output(STD, "Pass --exec , --in and --out for file mode\n");
            return -1;
        }

        map->mode = MODE_FILE;
    }

    if(nFlag == TRUE)
    {
        if(aFlag == FALSE || tFlag == FALSE || oFlag == FALSE)
        {
            output(STD, "Pass --address , --port, --protocol, and --out for network mode\n");
            return -1;
        }

        map->mode = MODE_NETWORK;
    }

    if(sFlag == TRUE)
    {
        if(oFlag == FALSE)
        {
            output(STD, "Pass --in and --out for syscall mode\n");
            return -1;
        }

        map->mode = MODE_SYSCALL;
    }

    return 0;
}

static int create_shared(void **pointer, int size)
{
    *pointer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    if(*pointer == MAP_FAILED)
    {
        output(ERROR, "mmap: %s\n", strerror(errno));
        return -1;
    }

    return 0;
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

/**
 * We use this function to allocate a struct as shared memory and to initialize all the values.
 **/
static int intit_shared_mapping(struct shared_map **mapping)
{
    int rtrn;

    /* Memory map the struct as shared anonymous memory so we can share it with other processes. */
    rtrn = create_shared((void **)mapping, sizeof(struct shared_map));
    if(rtrn < 0)
    {
        output(ERROR, "Can't create shared object.\n");
        return -1;
    }

     /* Allocate the system call table. */
    rtrn = create_shared((void **)&map->sys_table, sizeof(struct syscall_table));
    if(rtrn < 0)
    {
        output(ERROR, "Can't create shared object\n");
        return -1;
    }

    /* We default to dumb_mode being off, otherwise known as smart mode.. */
    (*mapping)->dumb_mode = FALSE;

    /* Check if were in syscall mode, if we are set the socket_server_port. */
    if((*mapping)->mode == MODE_SYSCALL)
    {
        (*mapping)->socket_server_port = 0;
    }

    unsigned int core_count;

    /* Grab the core count of the machine we are on. */
    rtrn = get_core_count(&core_count);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get core count\n");
        return -1;
    }
    
    /* Set the number of child processes to the number of cores detected. */
    //(*mapping)->number_of_children = core_count;
    (*mapping)->number_of_children = 1; // Use one until bug with ctrl-c is fixed.

    /* Set running children to zero. */
    (*mapping)->running_children = 0;

    /* Set the stop flag to FALSE, when set to TRUE all processes start their exit routines and eventually exit. */
    atomic_init(&(*mapping)->stop, FALSE);

    /* Allocate the executable context object. */
    rtrn = create_shared((void **)&map->exec_ctx, sizeof(struct executable_context));
    if(rtrn < 0)
    {
        output(ERROR, "Can't create shared object\n");
        return -1;
    }

    /* Create the child process structures. */
    (*mapping)->children = malloc((*mapping)->number_of_children * sizeof(struct child_ctx *));
    if((*mapping)->children == NULL)
    {
        output(ERROR, "Can't create children object.\n");
        return -1;
    }

    unsigned int i;

    /* Loop for each child and allocate the child context object as shared anonymous memory. */
    for(i = 0; i < (*mapping)->number_of_children; i++)
    {
        struct child_ctx *child;

        rtrn = create_shared((void **)&child, sizeof(struct child_ctx));
        if(rtrn < 0)
        {
            output(ERROR, "Can't create shared object\n");
            return -1;
        }

        (*mapping)->children[i] = child;
        
        child->pid = EMPTY;
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

    /* We have to make sure that we were started with root privileges so that we can use dtrace an dother functionality. */
    rtrn = check_root();
    if(rtrn < 0)
    {
        output(ERROR, "Run as root.\n");
        return -1;
    }

    /* Create a shared memory map so that we can share state with other threads and procceses. */
    rtrn = intit_shared_mapping(&map);
    if(rtrn < 0)
    {
        output(ERROR, "Can't initialize.\n");
        return -1;
    }

    /* Parse the command line for user input. parse_cmd_line() will set variables
    in map, the shared memory object. This will tell the fuzzer how to run. */
    rtrn = parse_cmd_line(argc, argv);
    if(rtrn < 0)
    {
        output(ERROR, "Can't parse command line. \n");
        return -1;
    }

    /* Setup the program running enviroment. */
    rtrn = setup_runtime();
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup runtime enviroment. \n");
        return -1;
    }

    /* Start the main fuzzing loop. */
    rtrn = start_runtime();
    if(rtrn < 0)
    {
        output(ERROR, "Can't start runtime enviroment. \n");
        return -1;
    }
    
    return 0;
}
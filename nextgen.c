
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

static const char *optstring = "p:e:";

static struct option longopts[] = {

    { "path", required_argument, NULL, 'p' },
    { "exec", required_argument, NULL, 'e' },
    { "crypto", required_argument, NULL, 'c' },
    { NULL, 0, NULL, 0 }
};

static int parse_cmd_line(int argc, char *argv[])
{
    int ch, rtrn;
    int eFlag = 0, pFlag = 0;

    while((ch = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
    {
        switch(ch)
        {
            case 'e':
                rtrn = asprintf(&map->path_to_exec, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return -1;
                }

                eFlag = 1;

                break;

            case 'p':
                rtrn = asprintf(&map->path_to_dir, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return -1;
                }

                pFlag = 1;

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

            default:
              output(ERROR, "Unknown option\n");
              return -1;
        }
    }

    if(eFlag == 0 || pFlag == 0)
    {
        output(STD, "No args passed\n");
        return -1;
    }

    return 0;
}

/** Entry point to the program. */
int main(int argc, char *argv[])
{
    int rtrn;

    /* Create a shared memory map so that we can share state with other threads and procceses. */
    rtrn = create_shared((void **)&map, sizeof(struct shared_map));
    if(rtrn < 0)
    {
    	output(ERROR, "Can't create shared object.\n");
    	return -1;
    }

    /* Parse the command line for user input. parse_cmd_line() will set variables
    in map, the shared memory object. */
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

    /* Start the main loop. */
    rtrn = start_runtime();
    if(rtrn < 0)
    {
        output(ERROR, "Can't start runtime enviroment. \n");
        return -1;
    }
    
    return 0;
}
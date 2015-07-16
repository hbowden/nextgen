
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

    { "in", required_argument, NULL, 'i' },
    { "out", required_argument, NULL, 'o' },
    { "exec", required_argument, NULL, 'e'},
    { "file", 0, NULL, 'f'},
    { "network", required_argument, NULL, 'n'},
    { "syscall", required_argument, NULL, 's'},
    { "crypto", required_argument, NULL, 'c' },
    { NULL, 0, NULL, 0 }
};

static int parse_cmd_line(int argc, char *argv[])
{
    int ch, rtrn;
    int iFlag = 0, oFlag = 0, fFlag = 0, nFlag = 0, sFlag = 0, eFlag = 0;

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

            case 'i':
                rtrn = asprintf(&map->path_to_in_dir, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return -1;
                }

                iFlag = 1;
                break;

            case 'o':
                rtrn = asprintf(&map->path_to_out_dir, "%s", optarg);
                if(rtrn < 0)
                {
                    output(ERROR, "asprintf: %s\n", strerror(errno));
                    return -1;
                }

                oFlag = 1;
                break;

            case 'f':
                fFlag = 1;
                map->mode = MODE_FILE;
                break;

            case 'n':
                nFlag = 1;
                map->mode = MODE_NETWORK;
                break;

            case 's':
                sFlag = 1;
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

            default:
              output(ERROR, "Unknown option\n");
              return -1;
        }
    }

    /* Make sure a fuzzing mode was selected. */
    if(fFlag != 1 && nFlag != 1 && sFlag != 1)
    {
        output(STD, "Specify a fuzzing mode\n");
        return -1;
    }

    /* If file mode was selected lets make sure all the right ars were passed.*/
    if(fFlag == 1)
    {
        if(iFlag == 0 || oFlag == 0 || eFlag == 0)
        {
            output(STD, "Pass --exec , --in and --out for file mode\n");
            return -1;
        }
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

/* Entry point to the program. */
int main(int argc, char *argv[])
{
    int rtrn;

    /* We have to make sure that we were started with root privileges so that we can use dtrace. */
    rtrn = check_root();
    if(rtrn < 0)
    {
        output(ERROR, "Run as root.\n");
        return -1;
    }

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
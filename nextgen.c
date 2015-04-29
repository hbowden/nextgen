
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <dtrace.h>
#include <sys/mman.h>

struct shared_map
{
    char *path_to_dir;
    char *path_to_exec;
    char *bin_start_addr;
    char *bin_end_addr;
};

/*static int start_reaper(void)
{

	return 0;
}*/

static int create_shared(void **pointer, int size)
{
    *pointer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    if(*pointer == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }

    return 0;
}

static const char *optstring = "p:e:";

static struct option longopts[] = {

    { "path", required_argument, NULL, 'p'},
    { "exec", required_argument, NULL, 'e'},
    { NULL, 0, NULL, 0 }
};

static int parse_cmd_line(int argc, char *argv[], struct shared_map *map)
{
    int ch, rtrn;

    while((ch = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
    {
        switch(ch)
        {
            case 'e':
                rtrn = asprintf(&map->path_to_exec, "%s", optarg);
                if(rtrn < 0)
                {
                    perror("asprintf");
                    return -1;
                }
                break;

            case 'p':
                rtrn = asprintf(&map->path_to_dir, "%s", optarg);
                if(rtrn < 0)
                {
                    perror("asprintf");
                    return -1;
                }
                break;

            default:
              printf("Unknown option\n");
              return -1;
        }
    }

    return 0;
}

/** Entry point to the program. */
int main(int argc, char *argv[])
{
	struct shared_map *map;
    int rtrn;

    rtrn = create_shared((void **)&map, sizeof(struct shared_map));
    if(rtrn < 0)
    {
    	printf("Can't create shared object.\n");
    	return -1;
    }

    rtrn = parse_cmd_line(argc, argv, map);
    if(rtrn < 0)
    {
        printf("Can't parse command line. \n");
        return -1;
    }


    
    
    return 0;
}
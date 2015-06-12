

#include "nextgen.h"
#include "reaper.h"
#include "utils.h"
#include <string.h>
#include <errno.h>

static int check_progess(void)
{
	return 0;
}

static void reaper(void)
{
    while(map->stop == FALSE)
    {
    	unsigned int i;

    	reap_dead_children();

    	unsigned int number_of_children = map->number_of_children;

    	for(i = 0; i <number_of_children; i++)
    	{
    		check_progess();
    	}
    }
	return;
}

int reap_dead_children(void)
{
	return 0;
}

int setup_and_run_reaper(void)
{
    map->reaper_pid = fork();
    if(map->reaper_pid == 0)
    {
    	reaper();
    }
    else if(map->reaper_pid > 0)
    {
    	return 0;
    }
    else
    {
    	output(ERROR, "Failed to fork reaper process: %s\n", strerror(errno));
    	return -1;
    }
	return 0;
}
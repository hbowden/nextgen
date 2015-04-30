

#include "runtime.h"
#include "utils.h"
#include "crypto.h"

static void set_up_signal_handler(void)
{

    return;
}

int set_up_runtime(void)
{
    unsigned int core_count;

    int rtrn = get_core_count(&core_count);
    if(rtrn < 0)
    {
    	printf("Can't get core count\n");
    	return -1;
    }

    set_up_signal_handler();

    /* This function checks to see if your cpu has crypto acceleration and if it does
    turn it on. */
    set_up_hardware_acceleration();

    /* Now lets pause the target binary, then inject dtrace probes into the target binary, then resume it. */
    rtrn = load_probes(map->path_to_exec);
    if(rtrn < 0)
    {
        printf("Can't load dtrace probes.")
    }


	return 0;
}
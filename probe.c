

#include "probe.h"
#include <dtrace.h>
#include "disas.h"

static int check_root(void)
{
    return 0;
}

/* Lets load dtrace probes into the target executable. */
int load_probes(char *path_to_exec)
{
    int rtrn;

    /* Make sure we have root so that we can use dtrace. */
    rtrn = check_root();
    if(rtrn != 0)
    {
        /* Exit because we don't have root. */
        printf("Run as root so we can probe the target executable.\n");
        return -1;
    }

    /* Disasemble the target binary so we can figure out the first and last address of the binary. */
    rtrn = disas_executable(path_to_exec);
    if(rtrn < 0)
    {
    	printf("Can't disas \n");
    }

    /* Lets create a dtrace handle. */
    dtrace_hdl_t *handle;

    handle = dtrace_open(DTRACE_VERSION, 0, &rtrn);
    if(handle == NULL)
    {
        printf("Can't create dtrace handle\n");
        return -1;
    }

    /* Set dtrace options. */


    /*  */




    return 0;
}
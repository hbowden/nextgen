

#include "runtime.h"
#include "utils.h"
#include "crypto.h"
#include "disas.h"
#include "reaper.h"
#include "nextgen.h"
#include <stdint.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

static void start_main_loop(void)
{

    return;
}

static void ctrlc_handler(int sig)
{

    sig = 0; // We do this to temprorarly silence a unused parameter warning.
    shutdown();
    
    return;
}

static void setup_signal_handler(void)
{
    signal(SIGINT, ctrlc_handler);

    return;
}

int setup_runtime(void)
{
    /* Lets grab the system's core count and use that for the number of child proceses. */
    int rtrn = get_core_count(&map->number_of_children);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't get core count\n");
    	return -1;
    }

    /* This function sets up the other crypto functions  */
    setup_crypto();

    /*  */
    uint32_t address;
    get_load_address(map->path_to_exec, &address);
    printf("address: %u\n", address);

    /* Lets set up the signal handler for the main process. */
    setup_signal_handler();

    rtrn = setup_and_run_reaper();
    if(rtrn < 0)
    {
    	output(ERROR, "Can't set up the reaper\n");
    	return -1;
    }

    return 0;
}

int start_runtime(void)
{
    map->runloop_pid = fork();
    if(map->runloop_pid == 0)
    {
    	start_main_loop();
    }
    else if(map->runloop_pid > 0)
    {
        int status;

        waitpid(map->runloop_pid, &status, 0);

        waitpid(map->reaper_pid, &status, 0);

        output(STD, "Exiting\n");

        return 0;
    }
    else
    {
    	output(ERROR, "runloop fork failed: %s\n", strerror(errno));
    	return -1;
    }

    return 0;
}

int shutdown(void)
{
    map->stop = 0;

    return 0;
}
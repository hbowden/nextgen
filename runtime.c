

#include "runtime.h"
#include "utils.h"
#include "crypto.h"
#include "disas.h"
#include "probe.h"
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
    map->stop = FALSE;

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
    output(STD, "Setting up fuzzer\n");

    /* Lets grab the system's core count and use that for the number of child proceses. */
    int rtrn = get_core_count(&map->number_of_children);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't get core count\n");
    	return -1;
    }

    /* Create executable context structure. */
    map->exec_ctx = malloc(sizeof(struct executable_context));
    if(map->exec_ctx == NULL)
    {
        output(ERROR, "Can't malloc space for exec_ctx: %s\n", strerror(errno));
        return -1;
    }

    /* This function sets up the other crypto functions and crypto library.  */
    setup_crypto();

    /* Lets parse the binary and figure out the virtual memory address it's going to be loaded at. */
    get_load_address();

     /* Lets inject dtrace probes into the target process. We will use these probes to calculate
     the code coverage of fuzzing proccess. */
    inject_probes();
    
    /* Inject the fork server. One can learn more about the fork server idea at:
    http://lcamtuf.blogspot.com/2014/10/fuzzing-binaries-without-execve.html .
    We use the fork server so that we can avoid the cost of dtrace probe injection and execv
    calls on each fuzz test. */
    inject_fork_server();

    /* Lets set up the signal handler for the main process. */
    setup_signal_handler();

    /* Now let's start the reaper, so it can clean up dead processes. */
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
    output(STD, "Starting fuzzer\n");

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
    map->stop = TRUE;

    return 0;
}
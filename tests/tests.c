/**
 * Copyright (c) 2015, Harrison Bowden, Minneapolis, MN
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

#include "test_utils.h"
#include "memory/memory.h"
#include "utils/utils.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

/* The number of test to run, keep in sync with test_paths  */
static uint32_t number_of_test = 13;

enum { MEMORY_TEST = 0, CONCURRENT_TEST = 1,
       CRYPTO_TEST = 2, UTILS_TEST = 3,
       NETWORK_TEST = 4, GENERATE_TEST = 5,
       RESOURCE_TEST = 6, REAPER_TEST = 7,
       SYSCALL_TEST = 8, FILE_TEST = 9, 
       GENETIC_TEST = 10, PLUGINS_TEST = 11,
       RUNTIME_TEST = 12
    };

/* Array of unit test file paths. */
static char *test_paths[] = {
    "tests/memory/test_memory",
    "tests/concurrent/test_concurrent",
    "tests/crypto/test_crypto",
    "tests/utils/test_utils",
    "tests/network/test_network",
    "tests/generate/test_generate",
    "tests/resource/test_resource",
    "tests/reaper/test_reaper",
    "tests/syscall/test_syscall",
    "tests/file/test_file",
    "tests/genetic/test_genetic",
    "tests/plugin/test_plugins",
    "tests/runtime/test_runtime",
    NULL   
};

static int32_t exec_test(char *path)
{
	int32_t rtrn = 0;
	pid_t exec_pid = 0;
    char *buf auto_free = NULL;
	char *args[] = { "blah", NULL };

    /* Make sure path is not NULL, if it is return an error. */
	if(path == NULL)
		return (-1);

    /* Create a child process. */
	exec_pid = fork();
    if(exec_pid == 0)
    {
        /* Set the PID of the currently running test. */
        test_stat->running_test = getpid();

    	/* Execute the test passed, with no arguments. */
        rtrn = execv(path, args);
        if(rtrn < 0)
        {
            rtrn = asprintf(&buf, "%s failed", path);
            if(rtrn < 0)
            {
                output(ERROR, "Can't create error string\n", strerror(errno));
                return (-1);
            }

            log_test(FAIL, buf);

            _exit(-1);
        }

        /* Exit and clean up the child process. */
        _exit(0);
    }
    else if(exec_pid > 0)
    {
    	int32_t status = 0;

        /* Wait for the child (test case) process. */
    	wait4(exec_pid, &status, 0, NULL);

        /* Make sure the process exited normally, if not return an error. */
    	if(WIFEXITED(status) != 0)
    	{
            if(WEXITSTATUS(status) != 0)
            {
                rtrn = kill(test_stat->running_test, SIGKILL);
                if(rtrn < 0)
                {
                    output(ERROR, "Can't kill target process: %s\n", strerror(errno));
                    return (-1);
                }
            }

            /* Return the exit status from the child process. */
            return (WEXITSTATUS(status));
    	}
    	else
    	{
    		rtrn = asprintf(&buf, "%s failed", path);
            if(rtrn < 0)
            {
                output(ERROR, "Can't create error string\n", strerror(errno));
                return (-1);
            }

            log_test(FAIL, buf);

            rtrn = kill(test_stat->running_test, SIGKILL);
            if(rtrn < 0)
            {
                output(ERROR, "Can't kill target process: %s\n", strerror(errno));
                return (-1);
            }

            return (-1);
    	}
    }
    else
    {
    	output(ERROR, "Failed to fork reaper process: %s\n", strerror(errno));
    	return (-1);
    }
}

static void run_single_test(char *argv[])
{
    int32_t rtrn = 0;

    if(strncmp(argv[1], "runtime", 7) == 0)
    {
        rtrn = exec_test(test_paths[RUNTIME_TEST]);
        if(rtrn < 0)
        {
            output(ERROR, "Can't exec unit test\n");
            return;
        }
    }
    else if(strncmp(argv[1], "genetic", 7) == 0)
    {
        rtrn = exec_test(test_paths[GENETIC_TEST]);
        if(rtrn < 0)
        {
            output(ERROR, "Can't exec unit test\n");
            return;
        }
    }
    else if(strncmp(argv[1], "syscall", 7) == 0)
    {
        rtrn = exec_test(test_paths[SYSCALL_TEST]);
        if(rtrn < 0)
        {
            output(ERROR, "Can't exec unit test\n");
            return;
        }
    }

    return;
}

int main(int argc, char *argv[])
{
    uint32_t i = 0;
    int32_t rtrn = 0;

    /* Create the stats object. */
    test_stat = create_stats_obj();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't create stats object\n");
        return (-1);
    }

    /* Set stats members to zero. */
    test_stat->fails = 0;
    test_stat->test_ran = 0;
    test_stat->successes = 0;
    test_stat->asserts_ran = 0;

    if(argc == 1)
    {
        output(STD, "Starting nextgen's test suite\n");
    }
    else
    {
        run_single_test(argv);
        return (0);
    }

    /* Loop and execute each test in the test array. */
    for(i = 0; i < number_of_test; i++)
    {
        /* Execute the unit test. */
    	rtrn = exec_test(test_paths[i]); 
    	if(rtrn < 0)
        {
            output(ERROR, "Can't exec unit test\n");
            /* Continue executing the test suite. */
    	    continue;
        }
    }

    /* Output results. */
    output(STD, "[%d] %ld successful assertions, %ld tests passed, and %ld tests failed.\n", \
          (100 * test_stat->successes) / test_stat->test_ran, test_stat->asserts_ran, test_stat->successes, test_stat->fails);

	return (0);
}

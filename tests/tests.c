

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

#include "test_utils.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/wait.h>

static char *test_paths[] = { "tests/crypto/test_crypto", 
                              "tests/memory/test_memory",
                              "tests/parser/test_parser", 
                              "tests/reaper/test_reaper",
                              "tests/resource/test_resource", 
                              "tests/syscall/test_syscall", 
                              NULL 
                            };

static uint32_t number_of_test = 6;

static int exec_test(char *path)
{
	int32_t rtrn = 0;
	pid_t exec_pid = 0;
	char *args[] = {"blah", NULL};

    /* Make sure path is not NULL, if it is return an error. */
	if(path == NULL)
		return -1;

    /* Create a child process. */
	exec_pid = fork();
    if(exec_pid == 0)
    {
    	/* Execute the test passed, with no arguments. */
        rtrn = execv(path, args);
        if(rtrn < 0)
        {
            output(ERROR, "Can't execute test case\n");
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
    		/* Return the exit status from the child process. */
            return (WEXITSTATUS(status));
    	}
    	else
    	{
    		output(ERROR, "Test: %s didn't exit normaly\n", path);
    		return -1;
    	}
    }
    else
    {
    	output(ERROR, "Failed to fork reaper process: %s\n", strerror(errno));
    	return -1;
    }
}

int main(void)
{
	output(STD, "Starting nextgen's test suite\n");

    uint32_t i = 0;
    int32_t rtrn = 0;

    /* Loop and execute each test in the test array. */
    for(i = 0; i < number_of_test; i++)
    {
    	rtrn = exec_test(test_paths[i]);
    	if(rtrn < 0)
    	    return -1;
    }

	return 0;
}



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

#include "child.h"
#include "log.h"
#include "io.h"
#include "concurrent.h"
#include "crypto.h"
#include "mutate.h"
#include "reaper.h"
#include "syscall.h"
#include "signals.h"
#include "nextgen.h"

#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

static void *kill_test_proc(void *pid)
{
    sleep(1);

    pid_t *proc_pid = (pid_t *)pid;

    int rtrn = kill(*proc_pid, SIGKILL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't kill child process: %s\n", strerror(errno));
        return NULL;
    }

    return NULL;
}

int test_exec_with_file_in_child(char *file_path, char *file_extension)
{
    int rtrn;
    pid_t child_pid;
    char *out_path auto_clean = NULL;
    char *file_name auto_clean = NULL;

    child_pid = fork();
    if(child_pid == 0)
    {
        if(atomic_load(&map->stop) == TRUE)
        {
             _exit(0);
        }

        char * const argv[] = {file_path, NULL};

        /* Execute the target executable with the file we generated. */
        rtrn = execv(map->exec_ctx->path_to_exec, argv);
        if(rtrn < 0)
        {
            output(ERROR, "Can't execute target program: %s\n", strerror(errno));
            return -1;
        }

        _exit(-1);
    }
    else if(child_pid > 0)
    {
        int status;
        pthread_t kill_thread;
        
        /* Create a thread to kill the other process if it does not crash. */
        rtrn = pthread_create(&kill_thread, NULL, kill_test_proc, &child_pid);
        if(rtrn < 0)
        {
            output(ERROR, "Can't create kill thread\n");
            return -1;
        }

        /* Wait for test program to crash or be killed by the kill thread. */
        waitpid(child_pid, &status, 0);

        /* Check if the target program recieved a signal. */
        if(WIFSIGNALED(status) == TRUE)
        {
            /* Check the signal the target program recieved. */
            switch(WTERMSIG(status))
            {
                /* We caused the signal so ignore it. */
                case SIGKILL:
                    break;

                /* The program we are testing crashed let's save the file
                that caused the crash.  */
                case SIGSEGV:
                case SIGBUS:

                    /* Create a random file name. */
                    rtrn = generate_name(&file_name, file_extension, FILE_NAME);
                    if(rtrn < 0)
                    {
                        output(ERROR, "Can't create out path: %s\n", strerror(errno));
                        return -1;
                    }

                    /* Create a file path.  */
                    rtrn = asprintf(&out_path, "%s/crash_dir/%s", file_path, file_name);
                    if(rtrn < 0)
                    {
                        output(ERROR, "Can't create out path: %s\n", strerror(errno));
                        return -1;
                    }

                    /* Copy file out of temp into the out directory. */
                    rtrn = copy_file_to(file_path, out_path);
                    if(rtrn < 0)
                    {
                        output(ERROR, "Can't copy file to crash directory\n");
                        return -1;
                    }

                    break;
                
                /* We recieved a less interesting signal let's save
                 that in a different directory. */
                default:
                   break;

            }
        }
        int *response;

        pthread_join(kill_thread, (void **)&response);
            
    }
    else
    {
        output(ERROR, "Can't create child process: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

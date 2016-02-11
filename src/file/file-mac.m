
#include "file-mac.h"
#include "memory/memory.h"
#include "utils/utils.h"
#include "io/io.h"

#include <signal.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

static void *kill_test_proc(void *kill_pid)
{
    /* Replace the sleep with a more sophisticated
    waiting mechanism. */
    sleep(1);

    pid_t *proc_pid = (pid_t *)kill_pid;

    int32_t rtrn = kill(*proc_pid, SIGKILL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't kill child process: %s\n", strerror(errno));
        return (NULL);
    }

    return (NULL);
}

static int32_t handle_crash(char *file_path, char *file_extension)
{
    int32_t rtrn = 0;
    char *path auto_clean = NULL;
    char *file_name auto_clean = NULL;

    /* Create a random file name. */
    rtrn = generate_name(&file_name, file_extension, FILE_NAME);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create out path: %s\n", strerror(errno));
        return (-1);
    }

    /* Create a file path.  */
    rtrn = asprintf(&path, "%s/crash_dir/%s", file_path, file_name);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create out path: %s\n", strerror(errno));
        return (-1);
    }

    /* Copy file out of temp into the out directory. */
    rtrn = copy_file_to(file_path, path);
    if(rtrn < 0)
    {
        output(ERROR, "Can't copy file to crash directory\n");
        return (-1);
    }

    return (0);
}

int32_t run_test_case(char *exec_path, char *file_path, char *file_extension)
{
    int32_t rtrn = 0;
    pid_t child_pid = 0;

    child_pid = fork();
    if(child_pid == 0)
    {
        /* Change our euid to 501 so we can bypass
        the apple sandbox. */
        rtrn = seteuid(501);
        if(rtrn < 0)
        {
            output(ERROR, "Can't seteuid\n");
            _exit(-1);
        }

        char * const argv[] = {file_path, NULL};

        /* Execute the target executable with the file we generated. */
        rtrn = execv(exec_path, argv);
        if(rtrn < 0)
        {
            output(ERROR, "Can't execute target program: %s\n", strerror(errno));
            _exit(-1);
        }

        _exit(0);
    }
    else if(child_pid > 0)
    {
        int32_t status = 0;
        pthread_t kill_thread;
        int32_t *response = NULL;
        
        /* Create a thread to kill the other process if it does not crash. */
        rtrn = pthread_create(&kill_thread, NULL, kill_test_proc, &child_pid);
        if(rtrn < 0)
        {
            output(ERROR, "Can't create kill thread\n");
            return (-1);
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
                    handle_crash(file_path, file_extension);
                    break;
                
                /* We recieved a less interesting signal let's save
                 that in a different directory. */
                default:
                   break;
            }
        }
        pthread_join(kill_thread, (void **)&response);   
    }
    else
    {
        output(ERROR, "Can't create child process: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

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

#include "probe.h"
#include "io/io.h"
#include "utils/utils.h"

#include <errno.h>
#include <mach/mach.h>
#include <mach/mach_types.h>

static pid_t pid;

static char *const args[] = {NULL, NULL};

static int32_t start_target(char *path)
{
    int32_t rtrn = 0;

    /* Get our PID. */
    pid = getpid();

    /* Drop root privileges. */
    rtrn = drop_privileges();
    if(rtrn < 0)
    {
        printf("Can't drop privileges\n");
        return (-1);
    }

    /* Now we execute the target binary. */
    rtrn = execv(path, args);
    if(rtrn < 0)
    {
        printf("execv: %d\n", errno);
        return (-1);
    }

    return (0);
}

static int32_t pause_target(pid_t target_pid)
{
    task_t target_task;
    kern_return_t kr = 0;

    /* Grab target process's task port. */
    kr = task_for_pid(mach_task_self(), target_pid, &target_task);
    if(kr != KERN_SUCCESS)
    {
        mach_error("Can't get target task port\n", kr);
        return (-1);
    }

    /* Pause the target process. */
    kr = task_suspend(target_task);
    if(kr != KERN_SUCCESS)
    {
        mach_error("Can't suspend target process\n", kr);
        return (-1);
    }

    return (0);
}

int32_t start_and_pause_target(char *path, pid_t *target_pid)
{
    int32_t rtrn = 0;

    pid = fork();
    if(pid == 0)
    {
        rtrn = start_target(path);
        if(rtrn < 0)
        {
            printf("Can't start target process\n");
            return (-1);
        }

        /* Exit child process and cleanup. */
        _exit(0);
    }
    else if(pid > 0)
    {
        rtrn = pause_target(pid);
        if(rtrn < 0)
        {
            printf("Can't pause target process\n");
            return (-1);
        }

        (*target_pid) = pid;

        return (0);
    }
    else
    {
        printf("Can't start and pause target process\n");
        return (-1);
    }
}

int32_t inject_fork_server(uint64_t main_address)
{
    (void)main_address;
    task_t target_task;
    kern_return_t kr = 0;

    /* Grab target process's task port. */
    kr = task_for_pid(mach_task_self(), pid, &target_task);
    if(kr != KERN_SUCCESS)
    {
        mach_error("Can't get target task port\n", kr);
        return (-1);
    }

    //mach_vm_read(target_task, ,op)

    return (0);
}

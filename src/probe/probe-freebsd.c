int32_t start_and_pause_target(void)
{
    int32_t rtrn = 0;

    /* Create child process. */
    target_pid = fork();
    if(target_pid == 0)
    {
        /* Get our PID. */
        target_pid = getpid();

        /* Drop root privileges. */
        rtrn = drop_privileges();
        if(rtrn < 0)
        {
            output(ERROR, "Can't drop privileges\n");
            return (-1);
        }
        /* Let's announce we want to be traced so that we don't execute any instructions on execv. */
        ptrace(PT_TRACE_ME, 0, NULL, 0);

        /* Now we execute the target binary. */
        rtrn = execv(target_path, args);
        if(rtrn < 0)
        {
            output(ERROR, "execv: %s\n", errno);
            return -1;
        }

        _exit(0);
    }
    else if(target_pid > 0)
    {
        int status;

        /* Wait until the target binary has stopped. */
        wait4(target_pid, &status, 0, NULL);

        /* Lets check the reason why were not waiting anymore. Hopefully it's
         because the target executable is stopped. */

        if(WIFCONTINUED(status) != 0)
        {
            output(ERROR, "The process we are testing is continuing?\n");
            return -1;
        }

        /* Check if the process exited. */
        if(WIFEXITED(status) != 0)
        {
            output(ERROR, "The process we are testing has exited prematurely\n");
            return -1;
        }

        /* Check if the process was terminated due to a signal. */
        if(WIFSIGNALED(status) != 0)
        {
            output(ERROR, "The process we are testing has exited due to a signal\n");
            return -1;
        }

        /* Check if the process exited stopped due to ptrace. This is the Macro that we hope
        evaluates to true. */
        if(WIFSTOPPED(status) != 0)
            goto EXIT;

        /* We should not get here. */
        return -1;
    }

EXIT:
    return 0;
}

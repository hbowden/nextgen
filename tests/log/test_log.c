/**
 * Copyright (c) 2016, Harrison Bowden, Minneapolis, MN
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
#include "log/log.c"
#include "syscall/syscall.c"

static int32_t test_write_arguments_to_log(void)
{
	log_test(DECLARE, "Testing write arguments to log");

	int32_t rtrn = 0;

    /* Create and initialize a child context object
    and make sure it's not NULL. */
	struct child_ctx *child = NULL;
    child = init_child_context();
    assert_stat(child != NULL);

    /* Pick a random syscall to generate arguments for. */
    rtrn = pick_syscall(child);
    assert_stat(rtrn == 0);

    /* Generate arguments to log. */
    rtrn = generate_arguments(child);
    assert_stat(rtrn == 0);

    /* If we pass an argument greater than the arg limit, 
    write_arguments_to_log() should fail. */
    rtrn = write_arguments_to_log(ARG_LIMIT + 9, 
                                  child->arg_value_array, 
                                  child->syscall_number);
    assert_stat(rtrn < 0);

    /* If we pass a NULL pointer for a the arg value array, 
    write_arguments_to_log() should fail. */
    rtrn = write_arguments_to_log(child->total_args, 
                                  NULL, 
                                  child->syscall_number);
    assert_stat(rtrn < 0);

    /* If we pass a syscall number greater than total, 
    write_arguments_to_log() should fail. */
    rtrn = write_arguments_to_log(child->total_args, 
                                  child->arg_value_array, 
                                  sys_table->number_of_syscalls + 7);
    assert_stat(rtrn < 0);

    /* Log the arguments generated earlier to the log file
    with real arguments. */
	rtrn = write_arguments_to_log(child->total_args, 
		                          child->arg_value_array, 
		                          child->syscall_number);
	assert_stat(rtrn == 0);

    FILE *fp = log;

    size_t ret = 0;
    uint32_t version = 0;

    /* Check for version. */
    ret = fread(&version, sizeof(uint32_t), 1, fp);
    assert_stat(ferror(fp) == 0);
    assert_stat(version == LATEST_LOG_VERSION);

    struct log_entry entry;

    ret = fread(&entry, sizeof(struct log_entry), 1, fp);
    printf("og: %u\n", child->syscall_number);
    printf("num: %u\n", entry.syscall_number);
    assert_stat(ferror(fp) == 0);
    assert_stat(entry.syscall_number == child->syscall_number);
    assert_stat(entry.arg_value_array != NULL);
    assert_stat(entry.total_args == child->total_args);

	log_test(SUCCESS, "Write arguments to log test passed");

	return (0);
}

static int32_t test_setup_log_module(void)
{
	log_test(DECLARE, "Testing setup log module");

    int32_t rtrn = 0;

    /* Path of where to create the logging directory. */
    char *dir = "/tmp/results";

    delete_directory(dir);

    /* Pass the path to the setup function. */
    rtrn = setup_log_module(dir, sys_table->number_of_syscalls);
    assert_stat(rtrn == 0);

    struct stat sb;

    rtrn = stat(dir, &sb);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get stats: %s\n", strerror(errno));
        return(-1);
    }

    /* Make sure the path is a directory. */
    if(sb.st_mode & S_IFDIR)
    {

    }
    else
    {
        /* Purposely fail because the path is not a directory. */
        assert_stat(1 == 2);
    }

    FILE *fp = NULL;

    /* There should be a log file at /tmp/results/log.nx, let's check. */
    fp = fopen("/tmp/results/log.nx", "w+");
    assert_stat(fp != NULL);

    size_t ret = 0;
    uint32_t version = 0;

    /* Check for version. */
    ret = fread(&version, sizeof(uint32_t), 1, fp);
    assert_stat(ferror(fp) == 0);
    assert_stat(version == LATEST_LOG_VERSION);

    rtrn = delete_directory(dir);
    if(rtrn < 0)
    {
        output(ERROR, "Can't remove directory\n");
        return (-1);
    }

	log_test(SUCCESS, "Setup log module test passed");

	return (0);
}

static int32_t test_create_log_file(void)
{
	log_test(DECLARE, "Testing create log file");

    int32_t rtrn = 0;
    FILE *fp = NULL;

    rtrn = create_log_file("/tmp/log.nx");
    assert_stat(rtrn == 0);

    fp = fopen("/tmp/log.nx", "w+");
    assert_stat(fp != NULL);

    size_t ret = 0;
    uint32_t version = 0;

    /* Check for version. */
    ret = fread(&version, sizeof(uint32_t), 1, fp);
    assert_stat(ferror(fp) == 0);
    assert_stat(version == LATEST_LOG_VERSION);

	log_test(SUCCESS, "Create log file test passed");

	return (0);
}

int main(void)
{
    int32_t rtrn = 0;

    /* Initialize the test runner. */
    test_stat = init_test_framework();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't init test framework");
        return (-1);
    }

    /* Declare the atomic stop pointer and atomic counter.
    The stop pointer is used to control the syscall module.
    If the integer pointed to by stop_ptr is TRUE then the syscall
    module process exits. */
    atomic_int_fast32_t stop_ptr;
    atomic_uint_fast64_t counter;

    /* Initialize the atomic variables before use. */
    atomic_init(&stop_ptr, FALSE);
    atomic_init(&counter, 0);

    /* Setup the crypto module so we can use the syscall module. */
    rtrn = setup_crypto_module(CRYPTO);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't setup crypto module\n");
    	return (-1);
    }

    /* We need to init the resource module before using
    the syscall module. */
    rtrn = setup_resource_module("/tmp");
    if(rtrn < 0)
    {
        output(ERROR, "Can't setup resource module");
        return (-1);
    }

    /* Setup the syscall module so we can generate syscall arguments
     for testing the logging functions. */
    rtrn = setup_syscall_module(&stop_ptr, &counter, TRUE);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't setup crypto module\n");
    	return (-1);
    }

    rtrn = test_setup_log_module();
    if(rtrn < 0)
        log_test(FAIL, "Setup log module test failed\n");

    rtrn = test_create_log_file();
    if(rtrn < 0)
        log_test(FAIL, "Create log file test failed\n");

    rtrn = test_write_arguments_to_log();
    if(rtrn < 0)
        log_test(FAIL, "Write arguments to log test failed");

    _exit(0);
}

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

#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include "test_utils.h"
#include "runtime/platform.h"
#include "resource/resource.h"
#include "crypto/crypto.h"
#include "syscall/generate.h"
#include "syscall/context.h"
#include "network/network.h"

#include "../../src/syscall/syscall.c"

static uint32_t iterations = 10000;

static int32_t test_generate_fd(void)
{
	log_test(DECLARE, "Testing generate file descriptor");

    int32_t *fd = NULL;
    int32_t rtrn = 0;
    uint32_t i;
    struct child_ctx *child = NULL;

    child = init_child_context();
	assert_stat(child != NULL);

    for(i = 0; i < iterations; i++)
    {
        rtrn = generate_fd((uint64_t **)&fd, child);
        assert_stat(rtrn == 0);
        assert_stat((*fd) > 0);
        assert_stat(child->arg_size_array[0] > 0);

        /* Make sure we can write to the descriptor. */
        assert_stat(write((*fd), "TEST", 4) == 4);

        /* Free the desc. */
        free_desc(fd);
    }

    log_test(SUCCESS, "Generate file descriptor test passed");

	return (0);
}

static int32_t test_generate_socket(void)
{
	log_test(DECLARE, "Testing generate socket");

    int32_t *fd = 0;
    int32_t rtrn = 0;
    uint32_t i;

    struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

    for(i = 0; i < iterations; i++)
    {
        rtrn = generate_socket((uint64_t **)&fd, child);
        assert_stat(rtrn == 0);
        assert_stat((*fd) > 0);
        assert_stat(child->arg_size_array[0] > 0);

        /* Make sure we can write to the descriptor. */
        assert_stat(write((*fd), "TEST", 4) == 4);

        free_socket(fd);
    }

    log_test(SUCCESS, "Generate socket test passed");

	return (0);
}

static int32_t test_generate_unmount_flags(void)
{
	log_test(DECLARE, "Testing generate unmount flags");

	int32_t rtrn = 0;
	int32_t *flag = NULL;
	uint32_t i;

	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_unmount_flags((uint64_t **)&flag, child);
	    assert_stat(rtrn == 0);
	    assert_stat(flag != NULL);
	    assert_stat((*flag) > 0);
	    assert_stat(child->arg_size_array[0] > 0);

	    mem_free((void **)&flag);
    }

	log_test(SUCCESS, "Generate unmount flags test passed");

	return (0);
}

static int32_t test_generate_request(void)
{
	log_test(DECLARE, "Testing generate request");

	int32_t rtrn = 0;
	int32_t *request = NULL;
	uint32_t i;

	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_request((uint64_t **)&request, child);
	    assert_stat(rtrn == 0);
	    assert_stat(request != NULL);
	    assert_stat((*request) >= 0);
	    assert_stat(child->arg_size_array[0] > 0);
	    mem_free((void **)&request);
    }

	log_test(SUCCESS, "Generate request test passed");

	return (0);
}

static int32_t test_generate_recv_flags(void)
{
	log_test(DECLARE, "Testing generate recv flags");

	int32_t rtrn = 0;
	int32_t *flag = NULL;

	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_recv_flags((uint64_t **)&flag, child);
	    assert_stat(rtrn == 0);
	    assert_stat(flag != NULL);
	    assert_stat((*flag) > 0);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate recv flags test passed");

	return (0);
}

/*static int32_t test_generate_mount_path(void)
{
	log_test(DECLARE, "Testing generate mount flags");

	int32_t rtrn = 0;
	char *mountpath = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_mountpath((uint64_t **)&mountpath, child);
	    assert_stat(rtrn == 0);
	    assert_stat(mountpath != NULL);
	    assert_stat(child->arg_size_index[0] > 0);
    }

	log_test(SUCCESS, "Generate mount flags test passed");

	return (0);
}*/

static int32_t test_generate_dev(void)
{
	log_test(DECLARE, "Testing generate dev_t");

    int32_t rtrn = 0;
	dev_t *dev = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_dev((uint64_t **)&dev, child);
	    assert_stat(rtrn == 0);
	    assert_stat(dev != NULL);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate dev_t test passed");

	return (0);
}

static int32_t test_generate_buf(void)
{
	log_test(DECLARE, "Testing generate buf");

    int32_t rtrn = 0;
	void *buf = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_buf((uint64_t **)&buf, child);
	    assert_stat(rtrn == 0);
	    assert_stat(buf != NULL);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate buf test passed");

	return (0);
}

static int32_t test_generate_length(void)
{
	log_test(DECLARE, "Testing generate length");

    int32_t rtrn = 0;
	uint64_t *len = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_length((uint64_t **)&len, child);
	    assert_stat(rtrn == 0);
	    assert_stat(len != NULL);
	    assert_stat((*len) > 0);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate length test passed");

	return (0);
}

static int32_t test_generate_path(void)
{
	log_test(DECLARE, "Testing generate path");

    int32_t rtrn = 0;
	char *path = NULL;
	struct child_ctx *child = NULL;
	int32_t fd = 0;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_path((uint64_t **)&path, child);
	    assert_stat(rtrn == 0);
	    assert_stat(path != NULL);
	    fd = open(path, O_RDONLY);
	    assert_stat(fd > 0);
	    assert_stat(child->arg_size_array[0] > 0);
	    close(fd);
	    free_filepath(&path);
    }

	log_test(SUCCESS, "Generate path test passed");

	return (0);
}

static int32_t test_generate_mode(void)
{
	log_test(DECLARE, "Testing generate mode");

    int32_t rtrn = 0;
	int32_t *mode_ptr = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_mode((uint64_t **)&mode_ptr, child);
	    assert_stat(rtrn == 0);
	    assert_stat(mode_ptr != NULL);
	    assert_stat((*mode_ptr) > 0);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate mode test passed");

	return (0);
}

static int32_t test_generate_open_flag(void)
{
	log_test(DECLARE, "Testing generate open flag");

    int32_t rtrn = 0;
	int32_t *flag = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_open_flag((uint64_t **)&flag, child);
	    assert_stat(rtrn == 0);
	    assert_stat(flag != NULL);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate open flag test passed");

	return (0);
}

static int32_t test_generate_fs_stat(void)
{
	log_test(DECLARE, "Testing generate fs stat flag");

    int32_t rtrn = 0;
	struct stat *buf = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_fs_stat((uint64_t **)&buf, child);
	    assert_stat(rtrn == 0);
	    assert_stat(buf != NULL);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate open flag test passed"); 

	return (0);
}

static int32_t test_generate_fs_stat_flag(void)
{
	log_test(DECLARE, "Testing generate fs stat flag");

    int32_t rtrn = 0;
	int32_t *flag = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_fs_stat_flag((uint64_t **)&flag, child);
	    assert_stat(rtrn == 0);
	    assert_stat(flag != NULL);
	    assert_stat((*flag) > 0);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate fs stat flag test passed"); 

	return (0);
}

static int32_t test_generate_dirpath(void)
{
	log_test(DECLARE, "Testing generate dirpath");

    int32_t rtrn = 0;
	char *dirpath = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_dirpath((uint64_t **)&dirpath, child);
	    assert_stat(rtrn == 0);
	    assert_stat(dirpath != NULL);
	    assert_stat(child->arg_size_array[0] > 0);
	    free_dirpath(&dirpath);
    }

	log_test(SUCCESS, "Generate dirpath test passed");

	return (0);
}

static int32_t test_generate_mount_type(void)
{
    log_test(DECLARE, "Testing generate mount type");

    int32_t rtrn = 0;
	int32_t *type = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_mount_type((uint64_t **)&type, child);
	    assert_stat(rtrn == 0);
	    assert_stat(type != NULL);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate mount type test passed");

    return (0);
}

static int32_t test_generate_offset(void)
{
    log_test(DECLARE, "Testing generate offset");

    int32_t rtrn = 0;
	int32_t *offset = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_offset((uint64_t **)&offset, child);
	    assert_stat(rtrn == 0);
	    assert_stat(offset != NULL);
	    assert_stat((*offset) >= 0);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate offset test passed");

    return (0);
}

static int32_t test_generate_whence(void)
{
    log_test(DECLARE, "Testing generate whence");

    int32_t rtrn = 0;
	int32_t *whence = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_whence((uint64_t **)&whence, child);
	    assert_stat(rtrn == 0);
	    assert_stat(whence != NULL);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate whence test passed");

    return (0);
}

static int32_t test_generate_wait_option(void)
{
    log_test(DECLARE, "Testing generate wait option");

    int32_t rtrn = 0;
	int32_t *option = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_wait_option((uint64_t **)&option, child);
	    assert_stat(rtrn == 0);
	    assert_stat(option != NULL);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate wait option test passed");

    return (0);
}

static int32_t test_generate_rusage(void)
{
    log_test(DECLARE, "Testing generate rusage");

    int32_t rtrn = 0;
	struct rusage *buf = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_rusage((uint64_t **)&buf, child);
	    assert_stat(rtrn == 0);
	    assert_stat(buf != NULL);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate rusage test passed");

    return (0);
}

static int32_t test_generate_int(void)
{
    log_test(DECLARE, "Testing generate integer");

    int32_t rtrn = 0;
	int32_t *number = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    for(i = 0; i < iterations; i++)
    {
	    rtrn = generate_int((uint64_t **)&number, child);
	    assert_stat(rtrn == 0);
	    assert_stat(number != NULL);
	    assert_stat((*number) >= 0);
	    assert_stat(child->arg_size_array[0] > 0);
    }

	log_test(SUCCESS, "Generate integer test passed");

    return (0);
}

static int32_t test_generate_pid(void)
{
    log_test(DECLARE, "Testing generate PID");

    int32_t rtrn = 0;
	int32_t *pid = NULL;
	struct child_ctx *child = NULL;

	child = init_child_context();
	assert_stat(child != NULL);

	uint32_t i;

    /* Only do 10 because creating PID's are expensive. */
    for(i = 0; i < 100; i++)
    {
	    rtrn = generate_pid((uint64_t **)&pid, child);
	    assert_stat(rtrn == 0);
	    assert_stat(pid != NULL);
	    assert_stat((*pid) >= 0);
	    assert_stat(child->arg_size_array[0] > 0);
	    kill((*pid), SIGKILL);
    }

	log_test(SUCCESS, "Generate PID test passed");

    return (0);
}

int main(void)
{
	int32_t rtrn = 0;

    test_stat = init_test_framework();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't init test framework");
        return (-1);
    }

    /* Setup the crypto and resource modules before
    using the generate_* functions.  */
    rtrn = setup_crypto_module(CRYPTO);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't setup crypto module\n");
    	return (-1);
    }

    rtrn = setup_resource_module(CACHE, "/tmp");
    if(rtrn < 0)
    {
    	output(ERROR, "Can't setup crypto module\n");
    	return (-1);
    }

    rtrn = test_generate_request();
    if(rtrn < 0)
        log_test(FAIL, "Generate request test failed");

    rtrn = test_generate_recv_flags();
    if(rtrn < 0)
        log_test(FAIL, "Generate recvieve flags test failed");

/*  rtrn = test_generate_mount_path();
    if(rtrn < 0)
        log_test(FAIL, "Generate mount path test failed"); */

    rtrn = test_generate_dev();
    if(rtrn < 0)
        log_test(FAIL, "Generate dev_t test failed");

    rtrn = test_generate_socket();
    if(rtrn < 0)
    	log_test(FAIL, "Generate socket test failed!");

    rtrn = test_generate_buf();
    if(rtrn < 0)
    	log_test(FAIL, "Generate buf test failed!");

    rtrn = test_generate_dirpath();
    if(rtrn < 0)
    	log_test(FAIL, "Generate dirpath test failed");

    rtrn = test_generate_offset();
    if(rtrn < 0)
    	log_test(FAIL, "Generate offset test failed");

    rtrn = test_generate_whence();
    if(rtrn < 0)
    	log_test(FAIL, "Generate whence test failed");

    rtrn = test_generate_wait_option();
    if(rtrn < 0)
    	log_test(FAIL, "Generate wait option test failed");

    rtrn = test_generate_rusage();
    if(rtrn < 0)
    	log_test(FAIL, "Generate rusage test failed");

    rtrn = test_generate_int();
    if(rtrn < 0)
    	log_test(FAIL, "Generate int test failed");

    rtrn = test_generate_pid();
    if(rtrn < 0)
    	log_test(FAIL, "Generate pid test failed");

    rtrn = test_generate_fs_stat_flag();
    if(rtrn < 0)
    	log_test(FAIL, "Generate fs stat flag test failed");

    rtrn = test_generate_fs_stat();
    if(rtrn < 0)
    	log_test(FAIL, "Generate fs stat test failed");

    rtrn = test_generate_open_flag();
    if(rtrn < 0)
    	log_test(FAIL, "Generate open flag test failed");

    rtrn = test_generate_mode();
    if(rtrn < 0)
        log_test(FAIL, "Generate mode test failed");

    rtrn = test_generate_path();
    if(rtrn < 0)
    	log_test(FAIL, "Generate path test failed");

    rtrn = test_generate_length();
    if(rtrn < 0)
    	log_test(FAIL, "Generate length test failed");

    rtrn = test_generate_fd();
    if(rtrn < 0)
    	log_test(FAIL, "Generate desc test failed!");

    rtrn = test_generate_unmount_flags();
    if(rtrn < 0)
        log_test(FAIL, "Generate unmount flags test failed");

    rtrn = test_generate_mount_type();
    if(rtrn < 0)
    	log_test(FAIL, "Generate mount type");

    _exit(0);
}

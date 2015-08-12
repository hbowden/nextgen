

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

#include "utils.h"
#include "nextgen.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/sysctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

int compare_and_swap_bool(atomic_bool *target, int value)
{
    /* Loop until we can succesfully update the the value. */
    while(1)
    {
        /* Grab a snapshot of the value that need to be updated. */
        bool snapshot = atomic_load(target);

        if(atomic_compare_exchange_weak(target, &snapshot, value) == TRUE)
        {
            /* We succesfully updated the value let's exit this loop and return. */
            break;
        }
    }

    return 0;
}

int compare_and_swap_uint32(atomic_uint_fast32_t *target, unsigned int value)
{
    /* Loop until we can succesfully update the the value. */
    while(1)
    {
        /* Grab a snapshot of the value that need to be updated. */
        unsigned int snapshot = atomic_load(target);

        if(atomic_compare_exchange_weak(target, &snapshot, value) == TRUE)
        {
            /* We succesfully updated the value let's exit this loop and return. */
            break;
        }
    }

    return 0;
}

int compare_and_swap_int32(atomic_int_fast32_t *target, int value)
{
    /* Loop until we can succesfully update the the value. */
    while(1)
    {
        /* Grab a snapshot of the value that need to be updated. */
        int snapshot = atomic_load(target);

        if(atomic_compare_exchange_weak(target, &snapshot, value) == TRUE)
        {
            /* We succesfully updated the value let's exit this loop and return. */
            break;
        }
    }

    return 0;
}

int compare_and_swap_uint64(atomic_uint_fast64_t *target, unsigned long value)
{
    /* Loop until we can succesfully update the the value. */
    while(1)
    {
        /* Grab a snapshot of the value that need to be updated. */
        unsigned long snapshot = atomic_load(target);

        if(atomic_compare_exchange_weak(target, &snapshot, value) == TRUE)
        {
            /* We succesfully updated the value let's exit this loop and return. */
            break;
        }
    }

    return 0;
}

int compare_and_swap_int64(atomic_int_fast64_t *target, long value)
{
    /* Loop until we can succesfully update the the value. */
    while(1)
    {
        /* Grab a snapshot of the value that need to be updated. */
        long snapshot = atomic_load(target);

        if(atomic_compare_exchange_weak(target, &snapshot, value) == TRUE)
        {
            /* We succesfully updated the value let's exit this loop and return. */
            break;
        }
    }

    return 0;
}

int get_file_size(int fd, off_t *size)
{
    struct stat stbuf;

    if((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
    {
        output(ERROR, "fstat: %s\n", strerror(errno));
        return -1;
    }

    *size = stbuf.st_size;

    return 0;
}

int map_file_in(int fd, char **buf, off_t *size)
{
	int rtrn;

    rtrn = get_file_size(fd, size);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't get file size\n");
    	return -1;
    }

    *buf = mmap(0, (unsigned long) *size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if(*buf == MAP_FAILED)
    {
    	output(ERROR, "mmap: %s\n", strerror(errno));
    	return -1;
    }

	return 0;
}

int map_file_out(char *path, char *buf, off_t *size)
{
    int fd;

    fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if(fd < 0)
    {
    	output(ERROR, "open: %s\n", strerror(errno));
    	return -1;
    }

    if(lseek(fd, *size - 1, SEEK_SET) == -1)
    {
    	output(ERROR, "lseek: %s\n", strerror(errno));
    	return -1;
    }

    ssize_t ret = write(fd, "", 1);
    if(ret != 1)
    {
    	output(ERROR, "write: %s\n", strerror(errno));
    	return -1;
    }

    char *dst = mmap(0, (unsigned long) size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if(dst == MAP_FAILED)
    {
    	output(ERROR, "mmap: %s\n", strerror(errno));
    	return -1;
    }

    memcpy(dst, buf, (unsigned long)size);

    munmap(dst, (unsigned long)size);

    close(fd);

	return 0;
}

void output(enum out_type type, const char *format, ...)
{
	va_list args;

	va_start(args, format);

	switch((int)type)
	{
		case ERROR:
		    vfprintf(stderr, format, args);
		    break;

		case STD:
		    vfprintf(stdout, format, args);
		    break;

        /* We default to stdout when ERROR or STD is not set so that
        we don't have any errors. */
		default:
		    vfprintf(stdout, format, args);
		    break;
	}

	va_end(args);

    (void)fflush(stdout);

	return;
}

int wait_on(atomic_int_fast32_t *pid, int *status)
{

    waitpid(atomic_load(pid), status, 0);

    return 0;
}

int get_core_count(unsigned int *core_count)
{
    int mib[4];

    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;

    size_t len = sizeof(unsigned int);

    int rtrn = sysctl(mib, 2, core_count, &len, NULL, 0);
    if(rtrn < 0)
    {
        output(ERROR, "sysctl: %s\n", strerror(errno));
        return -1;
    }

    if(*core_count < 1)
    {
    	*core_count = (unsigned int) 1;
    }

	return 0;
}
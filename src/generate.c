

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

#include "generate.h"
#include "nextgen.h"
#include "network.h"
#include "resource.h"
#include "memory.h"
#include "crypto.h"
#include "reaper.h"
#include "utils.h"
#include "io.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/mount.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int32_t generate_fd(uint64_t **fd)
{
    **fd = get_desc();
    if((int32_t)(**fd) < 0)
    {
        output(ERROR, "Can't get file descriptor\n");
        return (-1);
    }

	return (0);
}

int32_t generate_socket(uint64_t **sock)
{
    int rtrn;
    int socket_fd = 0;
    unsigned int number;
    
    rtrn = rand_range(3, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }

    *sock = mem_alloc(sizeof(unsigned long));
    if(*sock == NULL)
    {
        output(ERROR, "Can't allocate socket\n");
        return -1;
    }
    
    switch(number)
    {
        case 0:
            **sock = (unsigned long) socket(AF_INET6, SOCK_STREAM, 0);
            break;
            
        case 1:
            **sock = (unsigned long) socket(AF_INET, SOCK_STREAM, 0);
            break;
            
        case 2:

            rtrn = connect_ipv4(&socket_fd);
            if(rtrn < 0)
            {
                output(ERROR, "Can't connect to socket server: 4");
                return -1;
            }

            **sock = (unsigned long) socket_fd;

            break;
            
        case 3:

            rtrn = connect_ipv6(&socket_fd);
            if(rtrn < 0)
            {
                output(ERROR, "Can't connect to socket server: 6");
                return -1;
            }

            **sock = (unsigned long) socket_fd;

            break;

        default:
            output(ERROR, "Should not get here\n");
            return -1;
    }
    
    return 0;
}

int32_t generate_buf(uint64_t **buf)
{
    int rtrn;
    unsigned int number;
    unsigned int nbytes = 1024;

    rtrn = rand_range(2, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random number\n");
        return -1;
    }

    switch(number)
    {
        case 0:
            *buf = mmap(NULL, nbytes, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output(ERROR, "mmap: %s\n", strerror(errno));
                return -1;
            }
            break;

        case 1:
            *buf = mmap(NULL, nbytes, PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output(ERROR, "mmap: %s\n", strerror(errno));
                return -1;
            }
            break;

        case 2:
            *buf = mmap(NULL, nbytes, PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output(ERROR, "mmap: %s\n", strerror(errno));
                return -1;
            }
            break;

        default:
            output(ERROR, "Should not get here\n");
            return -1;
    }

	return 0;
}

int32_t generate_length(uint64_t **length)
{
	*length = mem_alloc(sizeof(unsigned long));
	if(*length == NULL)
    {
        output(ERROR, "Can't alloc length\n");
        return -1;
    }

	return 0;
}

int32_t generate_path(uint64_t **path)
{
    *path = (uint64_t *) get_filepath();
    if((*path) == NULL)
    {
        output(ERROR, "Can't get file path\n");
        return (-1);
    }
    
	return (0);
}

int32_t generate_open_flag(uint64_t **flag)
{
    int32_t rtrn;
    uint32_t number;
    
    rtrn = rand_range(7, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }

    *flag = mem_alloc(12);
    if(*flag == NULL)
    {
    	output(ERROR, "Can't alloc flag\n");
        return -1;
    }
    
    switch(number)
    {
        case 0: **flag |= (unsigned long) O_RDONLY; break;
            
        case 1: **flag |= (unsigned long) O_WRONLY; break;
      
        case 2: **flag |= (unsigned long) O_RDWR; break;

        case 3: **flag |= (unsigned long) O_NONBLOCK; break;
            
        case 4: **flag |= (unsigned long) O_APPEND; break;
            
        case 5: **flag |= (unsigned long) O_CREAT; break;
            
        case 6: **flag |= (unsigned long) O_TRUNC; break;

        case 7: **flag |= (unsigned long) O_EXCL; break;

        default:
            output(ERROR, "Should not get here\n");
            return -1;
    }
    
    return 0;
}

int32_t generate_mode(uint64_t **mode)
{
    int rtrn;
    unsigned int number;

    rtrn = rand_range(20, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }

    *mode = mem_alloc(12);
    if(*mode == NULL)
    {
    	output(ERROR, "Can't alloc mode\n");
        return -1;
    }
    
    switch(number)
    {
        case 0: **mode = 001; break;
            
        case 1: **mode = 002; break;
            
        case 2: **mode = 003; break;
            
        case 3: **mode = 004; break;
            
        case 4: **mode = 005; break;
            
        case 5: **mode = 006; break;
            
        case 6: **mode = 007; break;
            
        case 7: **mode = 010; break;
            
        case 8: **mode = 020; break;
            
        case 9: **mode = 030; break;
            
        case 10: **mode = 040; break;
            
        case 11: **mode = 050; break;
            
        case 12: **mode = 060; break;
            
        case 13: **mode = 070; break;
            
        case 14: **mode = 100; break;
            
        case 15: **mode = 200; break;
            
        case 16: **mode = 300; break;
            
        case 17: **mode = 400; break;
            
        case 18: **mode = 500; break;
            
        case 19: **mode = 600; break;
            
        case 20: **mode = 700; break;
            
        default:
            output(ERROR, "Can't get mode\n");
            return -1;
    }
	return 0;
}

int32_t generate_fs_stat(uint64_t **stat)
{
	struct statfs *stat_buf = mem_alloc(sizeof(struct statfs));
    if(stat_buf == NULL)
    {
    	output(ERROR, "Can't alloc stat\n");
    	return -1;
    }

    *stat = (uint64_t *)stat_buf;

	return 0;
}

int32_t generate_fs_stat_flag(uint64_t **flag)
{
    int32_t rtrn;
    uint32_t number;

    *flag = mem_alloc(12);
    if(*flag == NULL)
    {
    	output(ERROR, "Can't alloc flag\n");
        return -1;
    }
    
    rtrn = rand_range(1, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }
    
    switch(number)
    {
        case 0: **flag = MNT_NOWAIT; break;

        case 1: **flag = MNT_WAIT; break;

        default:
            output(ERROR, "Can't pick stat flag\n");
            return -1;
    }

	return 0;
}

int32_t generate_pid(uint64_t **pid)
{
	pid_t local_pid;

	*pid = mem_alloc(sizeof(unsigned long));
	if(*pid == NULL)
	{
		output(STD, "Can't alloc pid\n");
		return -1;
	}

    local_pid = fork();
    if(local_pid == 0)
    {
        /* Loop sleep until we our killed after we are used for a syscall argument. */
        while(1)
        {
        	sleep(30);
        }
    }
    else if(local_pid > 0)
    {
        
    }
    else
    {
        output(ERROR, "Can't create pid: %s\n", strerror(errno));
        return -1;
    }

    **pid = (uint64_t)local_pid;
    
	return 0;
}

int32_t generate_int(uint64_t **integer)
{
	unsigned int number; 
    int rtrn;

	*integer = mem_alloc(sizeof(unsigned long));
	if(*integer == NULL)
	{
		output(ERROR, "Can't alloc int\n");
        return -1;
	}

    rtrn = rand_range(INT_MAX, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random int\n");
        return -1;
    }

	**integer = (unsigned long)number;

	return 0;
}

int32_t generate_rusage(uint64_t **usage)
{
    struct rusage *buf;

    buf = mem_alloc(sizeof(struct rusage));
    if(buf == NULL)
    {
    	output(ERROR, "Can't alloc rusage\n");
    	return -1;
    }

    *usage = (uint64_t *)buf;

	return 0;
}

int32_t generate_wait_option(uint64_t **option)
{
	int rtrn;
    unsigned int number;

    *option = mem_alloc(sizeof(unsigned long));
    if(*option == NULL)
    {
    	output(ERROR, "Can't alloc option\n");
        return -1;
    }
    
    rtrn = rand_range(1, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }
    
    switch(number)
    {
        case 0:
            **option = WUNTRACED;
            break;
            
        case 1:
            **option = WNOHANG;
            break;
            
        default:
            output(ERROR, "Should not get here\n");
            return -1;
    }

	return 0;
}

int32_t generate_whence(uint64_t **whence)
{
    int rtrn;
    unsigned int number;

    *whence = mem_alloc(sizeof(unsigned long));
    if(*whence == NULL)
    {
        output(ERROR, "mem_alloc\n");
        return -1;
    }

#ifdef FREEBSD

    rtrn = rand_range(4, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }

#else

    rtrn = rand_range(2, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }

#endif
    
    switch(number)
    {
        case 0:
            **whence = SEEK_SET;
            break;
            
        case 1:
            **whence = SEEK_CUR;
            break;

        case 2:
            **whence = SEEK_END;
            break;

    #ifdef FREEBSD

        case 3:
            **whence = SEEK_HOLE;
            break;

        case 4:
            **whence = SEEK_DATA;
            break;

    #else
         /* Nothing */
    #endif 
            
        default:
            output(ERROR, "Should not get here\n");
            return -1;
    }

    return 0;
}

int32_t generate_offset(uint64_t **offset)
{
    int32_t rtrn = 0;

    *offset = mem_alloc(sizeof(uint64_t));
    if(*offset == NULL)
    {
        output(ERROR, "mem_alloc\n");
        return -1;
    }
    
    rtrn = rand_range(INT_MAX, (uint32_t *)*offset);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }

    return 0;
}

int32_t generate_mount_path(uint64_t **path)
{
    return 0;
}

int32_t generate_mount_type(uint64_t **type)
{

    return 0;
}

int32_t generate_dirpath(uint64_t **dirpath)
{

    return 0;
}

int32_t generate_mount_flags(uint64_t **flag)
{

    return 0;
}

int32_t generate_unmount_flags(uint64_t **flag)
{
    return 0;
}

int32_t generate_request(uint64_t **flag)
{
    return 0;
}

int32_t generate_recv_flags(uint64_t **flag)
{
    return 0;
}

int32_t generate_dev(uint64_t **dev)
{
    return 0;
}

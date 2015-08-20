

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
#include "network.h"
#include "crypto.h"
#include "utils.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/mount.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int generate_fd(unsigned long **fd)
{
    int rtrn, file_desc;
    char *name auto_clean_buf = NULL;
    char *file_path auto_clean_buf = NULL;
    char *junk auto_clean_buf = NULL;

    rtrn = generate_name(&name, (char *)".txt", FILE_NAME);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate name\n");
        return -1;
    }
    
    rtrn = asprintf(&file_path, "/tmp/%s", name);
    if(rtrn < 0)
    {
        output(ERROR, "asprintf: %s\n", strerror(errno));
        return -1;
    }

    junk = malloc(4096);
    if(junk < 0)
    {
    	output(ERROR, "malloc: %s\n", strerror(errno));
        return -1;
    }

    rtrn = rand_bytes(&junk, 4095);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't create junk\n");
        return -1;
    }

    rtrn = map_file_out(file_path, junk, 4095);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't write junk to disk\n");
    	return -1;
    }

    file_desc = open(file_path, O_RDONLY, 777);
    if(file_desc < 0)
    {
        output(ERROR, "open: %s\n", strerror(errno));
        return -1;
    }

    *fd = malloc(sizeof(unsigned long));
    if(fd == NULL)
    {
        output(ERROR, "Can't alloc fd: %s\n", strerror(errno));
        return -1;
    }

    **fd = (unsigned long)file_desc;

	return 0;
}

int generate_socket(unsigned long **sock)
{
    int rtrn, socket_fd;
    unsigned int number;
    
    rtrn = rand_range(3, &number);
    if(rtrn < 0)
    {
        output(STD, "Can't generate random number\n");
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
    }
    
    return 0;
}

int generate_buf(unsigned long **buf)
{
	*buf = malloc(64);
	if(*buf == NULL)
	{
		output(ERROR, "Can't generate buf: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int generate_length(unsigned long **length)
{
	int rtrn;
	unsigned int size;

    rtrn = rand_range(1000, &size);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate length\n");
        return -1;
    }

	*length = malloc(sizeof(unsigned long));
	if(*length == NULL)
    {
        output(ERROR, "Can't alloc length: %s\n", strerror(errno));
        return -1;
    }
    
	return 0;
}

int generate_path(unsigned long **path)
{
    int rtrn;

    /* Temp variables that we define with auto_clean_buf so that we 
    don't have to worry about calling free. */
    char *file_name auto_clean_buf = NULL;
    char *junk auto_clean_buf = NULL;
    char *file_path = NULL;

    /* Use generate_name to create a random file name with the text extension.  */
    rtrn = generate_name((char **)&file_name, ".txt", FILE_NAME);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't generate file path\n");
    	return -1;
    }

    /* Join the file name with the home path. */
    rtrn = asprintf(&file_path, "/tmp/%s", file_name);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't join paths: %s\n", strerror(errno));
    	return -1;
    }

    /* Allocate a buffer to put junk in. */
    junk = malloc(4096);
    if(junk == NULL)
    {
        output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
        free(file_path);
    	return -1;
    }

    /* Put some junk in a buffer. */
    rtrn = rand_bytes(&junk, 4095);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
    	free(file_path);
    	return -1;
    }

    /* Write that junk to the file so that the file is not just blank. */
    rtrn = map_file_out(file_path, junk, 4095);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't write junk to disk\n");
    	free(file_path);
    	return -1;
    }

    /* If all went well let's set path to the file_path we just created. */
    *path = (unsigned long *)file_path;

	return 0;
}

int generate_open_flag(unsigned long **flag)
{
    int rtrn;
    unsigned int number;
    
    rtrn = rand_range(7, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }

    *flag = malloc(12);
    if(*flag == NULL)
    {
    	output(ERROR, "Can't alloc flag: %s\n", strerror(errno));
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

int generate_mode(unsigned long **mode)
{
    int rtrn;
    unsigned int number;

    rtrn = rand_range(20, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }

    *mode = malloc(12);
    if(*mode == NULL)
    {
    	output(ERROR, "Can't alloc mode: %s\n", strerror(errno));
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

int generate_fs_stat(unsigned long **stat)
{
	struct statfs *stat_buf = malloc(sizeof(struct statfs));
    if(stat_buf == NULL)
    {
    	output(ERROR, "Can't create stat struct: %s\n", strerror(errno));
    	return -1;
    }

    *stat = (unsigned long *)stat_buf;

	return 0;
}

int generate_fs_stat_flag(unsigned long **flag)
{
    int rtrn;
    unsigned int number;

    *flag = malloc(12);
    if(*flag == NULL)
    {
    	output(ERROR, "Can't alloc flag: %s\n", strerror(errno));
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

int generate_pid(unsigned long **pid)
{
	pid_t local_pid;

	*pid = malloc(sizeof(unsigned long));
	if(*pid == NULL)
	{
		output(STD, "pid_t: %s\n", strerror(errno));
		return -1;
	}

    local_pid = fork();
    if(local_pid == 0)
    {
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

    **pid = (unsigned long)local_pid;
    
	return 0;
}

int generate_int(unsigned long **integer)
{
	int number = 0;

	*integer = malloc(sizeof(unsigned long));
	if(*integer == NULL)
	{
		output(ERROR, "malloc: %s\n", strerror(errno));
        return -1;
	}

	**integer = (unsigned long)number;

	return 0;
}

int generate_rusage(unsigned long **usage)
{
    struct rusage *buf;

    buf = malloc(sizeof(struct rusage));
    if(buf == NULL)
    {
    	output(ERROR, "malloc: %s\n", strerror(errno));
    	return -1;
    }

    *usage = (unsigned long *)buf;

	return 0;
}

int generate_wait_option(unsigned long **option)
{
	int rtrn, number;

    *option = malloc(sizeof(unsigned long));
    if(*option == NULL)
    {
    	output(ERROR, "malloc: %s\n", strerror(errno));
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
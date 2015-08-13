

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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int generate_fd(unsigned long **fd)
{
	int rtrn;
    char *name, *file_path;
    ssize_t ret;

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
        free(name);
        return -1;
    }
    
    *fd = open(file_path, O_CREAT | O_TRUNC | O_RDWR, 777);
    if(*fd < 0)
    {
        output(ERROR, "open\n", strerror(errno));
        free(name);
        return -1;
    }
    
    ret = write(*fd, "73097984ye39p8yohruewnkf4897irghu43fg4fiub4", 4095);
    if(ret < 4095)
    {
        output(ERROR, "write: %s\n", strerror(errno));
        free(name);
        return -1;
    }
    
    free(file_path);

	return 0;
}

int generate_socket(unsigned long **sock)
{
    int rtrn, number, socket_fd;
    
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

	return 0;
}

int generate_path(unsigned long **path)
{
    int rtrn, fd;

    /* Temp variables that we define with auto_clean_buf so that we 
    don't have to worry about calling free. */
    char *home auto_clean_buf = NULL;
    char *file_name auto_clean_buf = NULL;
    char *file_path auto_clean_buf = NULL;
    char *junk auto_clean_buf = NULL;

    /* Use generate_name to create a random file name with the text extension.  */
    rtrn = generate_name((char **)&file_name, ".txt", FILE_NAME);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't generate file path\n");
    	return -1;
    }

    /* Grab the user's home directory path. */
    rtrn = get_home(&home);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't get your home directory path\n");
    	return -1;
    }

    /* Join the file name with the home path. */
    rtrn = asprintf(&file_path, "%s%s", home, file_name);
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
    	return -1;
    }

    /* Put some junk in a buffer. */
    rtrn = rand_bytes(&junk, 4095);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
    	return -1;
    }

    /* Write that junk to the file so that the file is not just blank. */
    rtrn = map_file_out(file_path, junk, 4095);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't write junk to disk\n");
    	return -1;
    }

    /* If all went well let's set path to the file_path we just created. */
    *path = (unsigned long *)file_path;

	return 0;
}

int generate_open_flag(unsigned long **flag)
{

	return 0;
}

int generate_mode(unsigned long **mode)
{

	return 0;
}

int generate_fs_stat(unsigned long **stat)
{
	return 0;
}

int generate_fs_stat_flag(unsigned long **flag)
{
	
	return 0;
}
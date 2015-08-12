

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
#include "utils.h"

int generate_fd(unsigned long *fd)
{

	return 0;
}

int generate_socket(unsigned long *socket)
{

	return 0;
}

int generate_buf(unsigned long *buf)
{

	return 0;
}

int generate_length(unsigned long *length)
{

	return 0;
}

int generate_path(unsigned long *path)
{
    int rtrn;

    rtrn = generate_name((char **)&path, ".txt", FILE_NAME);
    if(rtrn < 0)
    {
    	output(STD, "Can't generate file path\n");
    	return -1;
    }

	return 0;
}

int generate_open_flag(unsigned long *flag)
{

	return 0;
}
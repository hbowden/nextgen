

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

#include "utils.h"
#include "memory/memory.h"
#include "crypto/crypto.h"
#include "io/io.h"

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/sysctl.h>
#include <sys/mman.h>
#include <sys/syslimits.h>
#include <errno.h>
#include <pwd.h>

int32_t check_root(void)
{
    output(STD, "Making sure nextgen has root privileges\n");

    uid_t check = 0;

    check = getuid();
    if(check == 0)
        return (0);

    return (-1);
}

/* Drop root privileges. */
int32_t drop_privileges(void)
{
    int32_t rtrn = 0;

    rtrn = setgid(999);
    if(rtrn < 0)
    {
        output(ERROR, "Can't drop gid: %s\n", strerror(errno));
        return (-1);
    }

    rtrn = setuid(999);
    if(rtrn < 0)
    {
        output(ERROR, "Can't drop uid: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

int32_t get_extension(char *path, char **extension)
{
    char *pointer = strrchr(path, '.');
    if(pointer == NULL)
    {
        output(ERROR, "Can't find . :\n", strerror(errno));
        return (-1);
    }

    int32_t rtrn = asprintf(extension, "%s\n", pointer + 1);
    if(rtrn < 0)
    {
        output(ERROR, "Can't alloc extension: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

int32_t get_core_count(uint32_t *core_count)
{
    int32_t mib[4];

    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;

    size_t len = sizeof(uint32_t);

    int32_t rtrn = sysctl(mib, 2, core_count, &len, NULL, 0);
    if(rtrn < 0)
    {
        output(ERROR, "sysctl: %s\n", strerror(errno));
        return (-1);
    }

    if((*core_count) < 1)
    	(*core_count) = (uint32_t) 1;

	return (0);
}

int32_t generate_name(char **name, char *extension, enum name_type type)
{
    /* Declare some variables. */
    int32_t rtrn = 0;
    char *random_data auto_clean = NULL;
    char *tmp_buf auto_clean = NULL;
    
    /* Create some space in memory. */
    random_data = mem_alloc(PATH_MAX + 1);
    if(random_data == NULL)
    {
        output(ERROR, "Can't Allocate Space\n");
        return (-1);
    }
    
    /* Grab some random bytes. */
    rtrn = rand_bytes(&random_data, (PATH_MAX));
    if(rtrn < 0)
    {
        output(ERROR, "Can't get random bytes\n");
        return (-1);
    }

    /* Null terminate the random byte string. */
    random_data[PATH_MAX] = '\0';
    
    /* SHA256 hash the random output string. */
    rtrn = sha256(random_data, &tmp_buf);
    if(rtrn < 0)
    {
        output(ERROR, "Can't Hash Random Data\n");
        return (-1);
    }

    char *pointer = strrchr(extension, '.');
    
    switch((int32_t)type)
    {
        case DIR_NAME:
            rtrn = asprintf(name, "%s", tmp_buf);
            if(rtrn < 0)
            {
                output(ERROR, "Can't create dir path: %s\n", strerror(errno));
                return (-1);
            }
            break;
            
        case FILE_NAME:

            /* Check for a period in the extension string passed by the user. */
            if(pointer == NULL)
            {
                char *ext auto_clean = NULL;

                /* There's no period so let's create one. */
                rtrn = asprintf(&ext, ".%s", extension);
                if(rtrn < 0)
                {
                    output(ERROR, "Can't create extension string\n");
                    return (-1);
                }
                
                rtrn = asprintf(name, "%s%s", tmp_buf, ext);
                if(rtrn < 0)
                {
                    output(ERROR, "Can't create file path: %s\n", strerror(errno));
                    return (-1);
                }

                break;
            }

            rtrn = asprintf(name, "%s%s", tmp_buf, extension);
            if(rtrn < 0)
            {
                output(ERROR, "Can't create file path: %s\n", strerror(errno));
                return (-1);
            }
            break;
            
        default:
            output(ERROR, "Should not get here\n");
            return (-1);
    }

    return (0);
}

int32_t get_home(char **home)
{
    uid_t uid = 0;
    struct passwd *pwd = NULL;
    int32_t rtrn = 0;
    
    /* Get User UID */
    uid = getuid();
    
    /* Get user password struct */
    if(!(pwd = getpwuid(uid)))
    {
        output(ERROR, "Can't Get pwd struct: %s\n", strerror(errno));
        return (-1);
    }
    
    /* Copy User Home Directory to buffer supplied to function */
    rtrn = asprintf(home, "%s", pwd->pw_dir);
    if(rtrn < 0)
    {
        output(ERROR, "Can't Create home string: %s\n", strerror(errno));
        return (-1);  
    }
    
    /* Exit function. */
    return (0);
}

int32_t ascii_to_binary(char *input, char **out, uint64_t input_len, uint64_t *out_len)
{
    uint32_t i;

    /* Make sure length is not zero. */
    if(input_len == 0)
    {
        output(ERROR, "Length argument is zero\n");
        return (-1);
    }

    /* Calculate the output string length. 
     Should probably be switched to equivalent division. */
    (*out_len) = input_len * 8;

    /* Allocate the output buffer. */
    (*out) = mem_calloc((*out_len) + 1);
    if((*out) == NULL)
    {
        output(ERROR, "Can't allocate binary string\n");
        return (-1);
    }

    /* Loop and convert to a binary string. */
    for(i = 0; i < input_len; i++)
    {
        char ch = input[i];
        char *o = &(*out)[8 * i];
        unsigned char b;

        for (b = 0x80; b; b >>= 1)
            *o++ = ch & b ? '1' : '0';
    }
        
    /* null terminate the binary string. */
    (*out)[(*out_len)] = '\0';

    /* Return zero. */
    return (0);
}

int32_t create_random_file(char *root, char *ext, char **path, uint64_t *size)
{
    int32_t rtrn = 0;
    int32_t no_period = 0;
    char *name auto_clean = NULL;
    char *junk auto_clean = NULL;
    char *extension auto_clean = NULL;

    /* Check for a period in the extension string passed by the user. */
    char *pointer = strrchr(ext, '.');
    if(pointer == NULL)
    {
        /* There's no period so let's create one. */
        rtrn = asprintf(&extension, ".%s", ext);
        if(rtrn < 0)
        {
            output(ERROR, "Can't create extension string\n");
            return (-1);
        }

        no_period = 1;
    }

    if(no_period == 1)
    {
        /* Generate random file name. */
        rtrn = generate_name(&name, extension, FILE_NAME);
        if(rtrn < 0)
        {
            output(ERROR, "Can't generate random file name\n");
            return (-1);
        }
    }
    else
    {
        /* Generate random file name. */
        rtrn = generate_name(&name, ext, FILE_NAME);
        if(rtrn < 0)
        {
            output(ERROR, "Can't generate random file name\n");
            return (-1);
        }
    }
   
    /* Join paths together. */
    rtrn = asprintf(path, "%s/%s", root, name);
    if(rtrn < 0)
    {
        output(ERROR, "Can't join paths: %s\n", strerror(errno));
        return (-1);
    }

    /* Pick a random size between zero and 4 kilobytes. */
    rtrn = rand_range(4096, (uint32_t *)size);
    if(rtrn < 0)
    {
        output(ERROR, "Can't choose random number\n");
        return (-1);
    }

    /* Allocate a buffer to put junk in. */
    junk = mem_alloc((*size) + 1);
    if(junk == NULL)
    {
        output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
        return (-1);
    }

    /* Put some junk in a buffer. */
    rtrn = rand_bytes(&junk, (uint32_t)(*size));
    if(rtrn < 0)
    {
        output(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
        return (-1);
    }

    /* Write that junk to the file so that the file is not just blank. */
    rtrn = map_file_out((*path), junk, (*size));
    if(rtrn < 0)
    {
        output(ERROR, "Can't write junk to disk\n");
        return (-1);
    }

    return (0);
}

/*int32_t binary_to_ascii(char *input, char **out, uint64_t input_len, uint64_t *out_len)
{
    if(input_len == 0)
    {
        output(ERROR, "Input length is zero\n");
        return (-1);
    }

    (*out_len) = input_len / 8;

    (*out) = mem_calloc((*out_len) + 1);
    if((*out) == NULL)
    {
        output(ERROR, "Can't allocate binary string\n");
        return (-1);
    }
  
    (*out)[(*out_len)] = '\0';

    return (0);
} */

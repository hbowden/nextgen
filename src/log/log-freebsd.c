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

#include "log-freebsd.h" 

#include "io/io.h"
#include "memory/memory.h"

int32_t log_arguments(uint32_t number_of_args, 
                      const char *syscall_name,
	                  uint64_t **arg_value_array, 
                      struct arg_context **arg_context_array)
{
    char *arg_value auto_free = mem_alloc(1024);
    if(arg_value == NULL)
    {
       output(ERROR, "Can't create arg_value buffer\n");
       return (-1);
    }

    char *syscall_log_buf auto_free = mem_alloc(4096);
    if(syscall_log_buf == NULL)
    {
       output(ERROR, "Can't create syscall_log_buf buffer\n");
       return (-1);
    }

    sprintf(syscall_log_buf, "%s:", syscall_name);

    uint32_t i;

    for(i = 0; i < number_of_args; i++)
    {
    	switch((int32_t)arg_context_array[i]->log_type)
    	{
    		/* File and directory paths. */
    		case PATH:
    		    sprintf(arg_value, " %s=%s", arg_context_array[i]->name, (char *)arg_value_array[i]);
    		    break;

            /* Pointers. */
    		case POINTER:
    		    sprintf(arg_value, " %s=%p", arg_context_array[i]->name, (void *)arg_value_array[i]);
    		    break;

            /* Non pointer values. */
            case NUMBER:
                sprintf(arg_value, " %s=%lu", arg_context_array[i]->name, *(arg_value_array[i]));
                break;

            default:
                output(ERROR, "Unknown log type\n");
                return (-1);
    	}

    	strncat(syscall_log_buf, arg_value, strlen(arg_value));
    }
 
    output(STD, "%s\n", syscall_log_buf);
 
	return (0);
}

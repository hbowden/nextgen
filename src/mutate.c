

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

#include "mutate.h"
#include "nextgen.h"
#include "utils.h"
#include "crypto.h"
#include "arg_types.h"

int mutate_file(char *file, off_t *file_size)
{

    return 0;
}

static int mutate_int(unsigned long *integer)
{

	return 0;
}

static int mutate_length(unsigned long *length)
{

	return 0;
}

static int mutate_fd(unsigned long *fd)
{

	return 0;
}

static int mutate_buf(unsigned long **buf)
{

	return 0;
}

static int mutate_path(unsigned long **path)
{

	return 0;
}

int mutate_arguments(struct child_ctx *ctx)
{
	/* Our variables. */
	int rtrn;
	unsigned int i;
	unsigned int number_of_args = ctx->number_of_args;
    unsigned int number_of_args_to_mutate;
    unsigned int args_mutated = 0;
    unsigned int chance;

    /* Pick how many args to mutate. */
    rtrn = rand_range(number_of_args, &number_of_args_to_mutate);
    if(rtrn < 0)
    {
        output(ERROR, "Can't figure out how many args to mutate\n");
        return -1;
    }

    /* Loop until we have mutated the number of args selected to be mutated. */ 
    while(number_of_args_to_mutate < args_mutated)
    {
    	for(i = 0; i < number_of_args; i++)
    	{
            /* Check argument type. */
            switch((int)ctx->arg_type_index[i])
            {
            	case FILE_DESC:

            	    /* We only mutate file descriptors 30% of time, so pick a number. */
            	    rtrn = rand_range(9, &chance);
            	    if(rtrn < 0)
            	    {
            	    	output(ERROR, "Can't generate random number\n");
            	    	return -1;
            	    }

                    /* Chance could be zero so add one to compensate. */
                    chance = chance + 1;
                   
                    /* If chance is 3 or less let's fuzz the file descriptor. */
            	    if(chance >= 3)
            	    {
                        rtrn = mutate_fd(ctx->arg_value_index[i]);
                        if(rtrn < 0)
                        {
                            output(ERROR, "Can't mutate fd\n");
            	    	    return -1;
                        }

                        /* We fuzzed an arg so incremnent args_mutated. */
                        args_mutated++;

                        continue;
            	    }
            	    
            	    break;

            	case SIZE_T:

            	    /* We mutate length and size 50% of time, so pick a number. */
            	    rtrn = rand_range(9, &chance);
            	    if(rtrn < 0)
            	    {
            	    	output(ERROR, "Can't generate random number\n");
            	    	return -1;
            	    }

                    /* Chance could be zero so add one to compensate. */
                    chance = chance + 1;
                   
                    /* If chance is 5 or less let's fuzz the size or length arg. */
            	    if(chance >= 5)
            	    {
                        rtrn = mutate_length(ctx->arg_value_index[i]);
                        if(rtrn < 0)
                        {
                            output(ERROR, "Can't mutate fd\n");
            	    	    return -1;
                        }

                        /* We fuzzed an arg so incremnent args_mutated. */
                        args_mutated++;

                        continue;
            	    }
            	    
            	    break;

            	case VOID_BUFF:
            	case RUSAGE:
            	case STAT_FS:

            	    /* We mutate buffers and structs 50% of time. */
            	    rtrn = rand_range(9, &chance);
            	    if(rtrn < 0)
            	    {
            	    	output(ERROR, "Can't generate random number\n");
            	    	return -1;
            	    }

                    /* Chance could be zero so add one to compensate. */
                    chance = chance + 1;
                   
                    /* If chance is 5 or less let's fuzz the size or length arg. */
            	    if(chance >= 5)
            	    {
                        rtrn = mutate_buf(&ctx->arg_value_index[i]);
                        if(rtrn < 0)
                        {
                            output(ERROR, "Can't mutate fd\n");
            	    	    return -1;
                        }

                        /* We fuzzed an arg so incremnent args_mutated. */
                        args_mutated++;

                        continue;
            	    }
            	    
            	    break;

            	case FILE_PATH:

            	    /* We mutate file paths and dirpaths 50% of time. */
            	    rtrn = rand_range(9, &chance);
            	    if(rtrn < 0)
            	    {
            	    	output(ERROR, "Can't generate random number\n");
            	    	return -1;
            	    }

                    /* Chance could be zero so add one to compensate. */
                    chance = chance + 1;
                   
                    /* If chance is 5 or less let's fuzz the size or length arg. */
            	    if(chance >= 5)
            	    {
                        rtrn = mutate_path(&ctx->arg_value_index[i]);
                        if(rtrn < 0)
                        {
                            output(ERROR, "Can't mutate fd\n");
            	    	    return -1;
                        }

                        /* We fuzzed an arg so incremnent args_mutated. */
                        args_mutated++;

                        continue;
            	    }
            	    
            	    break;
            	
                case MODE_T:
                case OPEN_FLAG:
            	case STAT_FLAG:
            	case INT:
            	case PID:
            	case WAIT_OPTION:

            	     /* We mutate ints, flags and options 50% of time. */
            	    rtrn = rand_range(9, &chance);
            	    if(rtrn < 0)
            	    {
            	    	output(ERROR, "Can't generate random number\n");
            	    	return -1;
            	    }

                    /* Chance could be zero so add one to compensate. */
                    chance = chance + 1;
                   
                    /* If chance is 5 or less let's fuzz the size or length arg. */
            	    if(chance >= 5)
            	    {
                        rtrn = mutate_int(ctx->arg_value_index[i]);
                        if(rtrn < 0)
                        {
                            output(ERROR, "Can't mutate fd\n");
            	    	    return -1;
                        }

                        /* We fuzzed an arg so incremnent args_mutated. */
                        args_mutated++;

                        continue;
            	    }
            	    
            	    break;

            	default:
            	    output(ERROR, "Unknown type\n");
            	    return -1; 
            }
    	}
    }

	return 0;
}
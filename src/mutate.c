

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
#include "io.h"
#include "crypto.h"
#include "arg_types.h"
#include "plugin.h"

static int mutate_file_randomly(char **file)
{

    return 0;
}

int mutate_file(char **file, const char *file_extension,  off_t *file_size)
{
    int rtrn;
    bool understand_file_format = FALSE;
    unsigned int plugin_offset = 0;
    unsigned int number_of_features = 0;
    unsigned int feature = 0;
    unsigned int number = 0;

    /* Check if we understand the file format so we can provide context
    to the mutation engine. */
    if(suppoted_file(file_extension, &plugin_offset) == TRUE)
    {
        /* Some plugin understands this file format, let's use the plugin for more
        accurate fuzzing. Check how many features the plugin know's about. */
        features_supported(plugin_offset, &number_of_features);

        /* Sometimes randomly mutate the file instead of using the plugin. */
        rtrn = rand_range(10, &number);
        if(rtrn < 0)
        {
            output(ERROR, "Can't randomly select number\n");
            return -1;
        }

        /* If the number is equal to ten mutate the file completly randomly. */
        if(number == 10)
        {
            rtrn = mutate_file_randomly(file);
            if(rtrn < 0)
            {
                output(ERROR, "Can't mutate file randomly\n");
                return -1;
            }

            /* Exit early. */
            return 0;
        }

        /* Randomly select a file format feature to test. */
        rtrn = rand_range(number_of_features, &feature);
        if(rtrn < 0)
        {
            output(ERROR, "Can't randomly select feature to test\n");
            return -1;
        }

        struct feature_constraints *constraints = NULL;

        /* Get feature constraints.*/
        rtrn = feature_constraints(plugin_offset, constraints);
        if(rtrn < 0)
        {
            output(ERROR, "Can't get feature constraints\n");
            return -1;
        }

        

    }
    else
    {
        /* We don't know this file type, so let's just randomly mutate
        the file. */
        rtrn = mutate_file_randomly(file);
        if(rtrn < 0)
        {
            output(ERROR, "Can't mutate file randomly\n");
            return -1;
        }
    }

    return 0;
}

static char *random_junk(unsigned int length)
{
    char *address = NULL;

    return address;
}

static char *decrement_address(char *path)
{
    int rtrn;
    unsigned int number, i;

    rtrn = rand_range(16, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get random number\n");
        return -1;
    }

    for(i = 0; i < number; i++)
    {
        path--;
    }

    return path;
}

static char *increment_address(char *path)
{
    int rtrn;
    unsigned int number, i;

    rtrn = rand_range(32, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get random number\n");
        return -1;
    }

    for(i = 0; i < number; i++)
    {
        path++;
    }

    return path;
}

static char *random_address(void)
{
    unsigned int i;
    int rtrn, number;
    char address[13];

    for(i = 0; i < 12; i++)
    {
        rtrn = rand_range(14, &number);
        if(rtrn < 0)
        {
            output(ERROR, "Can't pick random number\n");
            return -1;
        }

        switch(number)
        {
            case 0: address[i] = 0; break;

            case 1: address[i] = 1; break;

            case 2: address[i] = 2; break;

            case 3: address[i] = 3; break;

            case 4: address[i] = 4; break;

            case 5: address[i] = 5; break;

            case 6: address[i] = 6; break;

            case 7: address[i] = 7; break;

            case 8: address[i] = 8; break;

            case 9: address[i] = 9; break;

            case 10: address[i] = 'a'; break;

            case 11: address[i] = 'b'; break;

            case 12: address[i] = 'c'; break;

            case 13: address[i] = 'd'; break;

            case 14: address[i] = 'f'; break;

            default:
                output(ERROR, "Should not get here\n");
                return -1;
        }
    }

    output(STD, "adr: %s\n", address);

    return address;
}

static char *kernel_address(void)
{
    char *address = NULL;

    return address;
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
    int rtrn, number;

    rtrn = rand_range(4, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick number\n");
        return -1;
    }

    switch(number)
    {
        case 0:
            *path = kernel_address();
            break;

        case 1:
            *path = random_address();
            break;

        case 2:
            *path = increment_address(*path);
            break;

        case 3:
            *path = decrement_address(*path);
            break;

        case 4:
            *path = random_junk(strlen(*path));
            break;

        default:
            output(ERROR, "Number to large\n");
            return -1;
    }

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


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

#include "test_utils.h"
#include "../src/memory.h"

struct stats *stat;

struct stats *init_stats_obj(void)
{
	struct stats *stat_obj = NULL;

    /* Allocate stats struct as shared memory. */
    stat_obj = mem_alloc_shared(sizeof(struct stats));
    if(stat_obj == NULL)
    {
    	output(ERROR, "Can't allocate stat struct\n");
    	return NULL;
    }

    /* Set stats members to zero. */
    stat_obj->fails = 0;
    stat_obj->successes = 0;
    stat_obj->test_ran = 0;
    stat_obj->asserts_ran = 0;

	return (stat_obj);
}

/* Function for logging test results. */
void log_test(enum log_type type, const char *input)
{
	switch((int)type)
	{
		case DECLARE:
            stat->test_ran++;
		    output(STD, "[%s]\n", input);
		    break;

		case SUCCESS:
		    stat->successes++;
		    output(STD, BOLD_GREEN"%s"RESET"\n", input);
		    break;

		case FAIL:
		    stat->fails++;
		    output(STD, BOLD_RED"%s\n"RESET"\n", input);
		    break;
	}
    
	return;
}

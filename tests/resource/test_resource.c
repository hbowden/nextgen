

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

static int test_resource(void)
{
	log_test(DECLARE, "Testing resource module");

	

    log_test(SUCCESS, "Resource module test passed");

    return 0;
}

int main(void)
{
	int rtrn = 0;

	/* Map the stats object as shared anonymous memory. */
    stat = init_stats_obj();
    if(stat == NULL)
    {
        output(ERROR, "Can't init the stats object\n");
        return -1;
    }

	rtrn = test_resource();
	if(rtrn < 0)
	    return -1;

	/* Output results. */
    output(STD, "[%d] %ld assertions passed, %ld test passed, and %ld test failed.\n", \
          (100 * stat->successes) / stat->test_ran, stat->asserts_ran, stat->successes, stat->fails);

	return 0;
}

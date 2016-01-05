

/**
 * Copyright (c) 2015, Harrison Bowden, Minneapolis, MN
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice 
 * and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY å
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **/

#include "test_utils.h"
#include "../../src/io.h"
#include "../../src/plugin.c"

#include <stdint.h>

static int32_t test_setup_plugin_module(void)
{
	log_test(DECLARE, "Testing setup plugin module");

	int32_t rtrn = 0;

	rtrn = setup_plugin_module();
	assert_stat(rtrn == 0);

    log_test(SUCCESS, "Setup plugin module test passed");

	return (0);
}

int main(void)
{
	int32_t rtrn = 0;

    /* Init the unit testing framework. */
    rtrn = init_test_framework();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init test framework");
        return (-1);
    }

    /* Initialize the stats object. */
    test_stat = init_stats_obj();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't init the stats object\n");
        return (-1);
    }

    rtrn = test_setup_plugin_module();
    if(rtrn)
    	log_test(FAIL, "Setup plugin module test failed");

    return (0);
}

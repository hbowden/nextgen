

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

#include "test_utils.h"
#include "../../src/resource.c"

static int test_get_fd(void)
{
	log_test(DECLARE, "Testing resource module");

    int32_t rtrn = 0;
    int32_t fd = 0;

    rtrn = get_desc(&fd);

    assert_stat(rtrn == 0);
    assert_stat(fd > 0);
	
    log_test(SUCCESS, "Resource module test passed");

    return 0;
}

static int test_setup_resource_module(void)
{
    log_test(DECLARE, "Testing resource module setup");

    int32_t rtrn = 0;

    rtrn = create_file_pool();

    assert_stat(rtrn == 0);
    assert_stat(file_pool != NULL);

    struct memory_block *m_blk = NULL;

    CK_SLIST_FOREACH(m_blk, &file_pool->allocated_list, list_entry)
    {
        assert_stat(m_blk != NULL);
        assert_stat(m_blk->ptr != NULL);
    }
    
    log_test(SUCCESS, "Resource module setup test passed");

    return (0);
}

int main(void)
{
	int32_t rtrn = 0;

    rtrn = init_test_framework();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init test framework");
        return (-1);
    }

    /* Initialize the stats object. */
    stat = init_stats_obj();
    if(stat == NULL)
    {
        output(ERROR, "Can't init the stats object\n");
        return (-1);
    }

    rtrn = test_setup_resource_module();
    if(rtrn < 0)
    {
        log_test(FAIL, "setup resource module failed");
        return (-1);
    }

	rtrn = test_get_fd();
	if(rtrn < 0)
    {
        log_test(FAIL, "get fd test failed");
	    return -1;
    }

	return 0;
}

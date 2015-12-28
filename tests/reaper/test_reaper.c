

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

#include "../../src/reaper.h"
#include "../../src/syscall.h"
#include "../../src/nextgen.h"
#include "../../src/memory.h"
#include "test_utils.h"

static int32_t test_reaper(void)
{
	log_test(DECLARE, "Testing reaper module");

    int rtrn;
    pid_t reaper_pid;

    rtrn = setup_reaper_module(&reaper_pid);
    assert_stat(rtrn == 0);
    assert_stat(reaper_pid > 0);

    struct child_ctx *child = NULL;

    unsigned int i = 0;

    for(i = 0; i < number_of_children; i++)
    {
        child = get_child_from_index(i);
        assert_stat(child != NULL);
    }

	log_test(SUCCESS, "Reaper test passed");

	return 0;
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

    /* The map must be alloced as shared memory for reaper to work. */
    map = mem_alloc_shared(sizeof(struct shared_map));
    if(map == NULL)
    {
        output(ERROR, "Can't create shared object.\n");
        return (-1);
    }
    
    /* Init the stop atomic variable. */
    atomic_init(&map->stop, FALSE);

    rtrn = test_reaper();
    if(rtrn < 0)
        log_test(FAIL, "Reaper test failed");

    return (0);
}

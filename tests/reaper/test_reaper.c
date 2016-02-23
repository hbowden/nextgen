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

#include "reaper/reaper.h"
#include "syscall/syscall.h"
#include "memory/memory.h"

#include <signal.h>
#include "stdatomic.h"
#include "test_utils.h"

static pid_t reaper_pid;

static int32_t test_reaper(void)
{
	log_test(DECLARE, "Testing reaper module");

    int32_t rtrn = 0;

    atomic_int_fast32_t stop;
    
    /* Init the stop atomic variable. */
    atomic_init(&stop, FALSE);

    rtrn = setup_reaper_module(&reaper_pid, &stop);
    assert_stat(rtrn == 0);
    assert_stat(reaper_pid > 0);

    struct child_ctx *child = NULL;

    uint32_t i = 0;

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

    test_stat = init_test_framework();
    if(test_stat == NULL)
    {
        output(ERROR, "Can't init test framework");
        return (-1);
    }

    rtrn = test_reaper();
    if(rtrn < 0)
        log_test(FAIL, "Reaper test failed");


    /* Kill the reaper process. */
    kill(reaper_pid, SIGKILL);

    _exit(0);
}

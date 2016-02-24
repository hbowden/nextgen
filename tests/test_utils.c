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
#include "memory/memory.h"
#include "utils/utils.h"

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

struct stats *test_stat = NULL;

static const char *name = "/shared_memory";

static struct stats *init_stats_obj(void)
{
	int32_t fd = 0;

    /* Create named shared memory. */
    fd = shm_open(name, O_RDWR, 0666);
    if(fd < 0)
    {
        output(ERROR, "Can't create named shared memory\n");
        return (NULL);
    }

    /* Map named shared object. */
    test_stat = mmap(NULL, sizeof(struct stats), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(test_stat == MAP_FAILED)
    {
        output(ERROR, "Can't mmap shared object: %s\n", strerror(errno));
        return (NULL);
    }

	return (test_stat);
}

struct stats *create_stats_obj(void)
{
	int32_t fd = 0;

    /* Delete the old shared memory object.  */
    shm_unlink(name);

    /* Create named shared memory. */
    fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if(fd < 0)
    {
        output(ERROR, "Can't create named shared memory: %s\n", strerror(errno));
        return (NULL);
    }

    if(ftruncate(fd, sizeof(struct stats)) == -1)
    {
    	output(ERROR, "Can't set size: %s\n", strerror(errno));
    	return (NULL);
    }

    /* Map named shared object. */
    test_stat = mmap(NULL, sizeof(struct stats), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(test_stat == MAP_FAILED)
    {
        output(ERROR, "Can't mmap shared object: %s\n", strerror(errno));
        return (NULL);
    }

	return (test_stat);
}
  
/* Function for logging test results. */
int32_t log_test(enum log_type type, const char *input)
{
	switch((int32_t)type)
	{
		case DECLARE:
            test_stat->test_ran++;
		    output(STD, "[%s]\n", input);
            break;

		case SUCCESS:
		    test_stat->successes++;
		    output(STD, BOLD_GREEN"%s"RESET"\n", input);
		    break;

		case FAIL:
		    test_stat->fails++;
		    output(STD, BOLD_RED"%s"RESET"\n", input);
		    break;
	}
	return (0);
}

struct stats *init_test_framework(void)
{
    /* Return a stats object.  */
	return (init_stats_obj());
}

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
#include "../src/memory/memory.h"
#include "../src/utils/utils.h"

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

struct stats *test_stat = NULL;

static char *shared_path = NULL;

struct stats *init_stats_obj(void)
{
	int32_t fd = 0;

    if(shared_path == NULL)
    {
    	output(ERROR, "Call init_test_framework() first.\n");
    	return NULL;
    }

#ifdef MAC_OSX

    /* Create named shared memory. */
    fd = shm_open(shared_path, O_RDWR);
    if(fd < 0)
    {
        output(ERROR, "Can't create named shared memory\n");
        return NULL;
    }

#endif
#ifdef FREEBSD

    /* Create named shared memory. */
    fd = shm_open(shared_path, O_RDWR, 0666);
    if(fd < 0)
    {
        output(ERROR, "Can't create named shared memory\n");
        return (NULL);
    }

#endif
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

    if(shared_path == NULL)
    {
    	output(ERROR, "Call init_test_framework() first.\n");
    	return (NULL);
    }

    shm_unlink(shared_path);

    /* Create named shared memory. */
    fd = shm_open(shared_path, O_CREAT | O_RDWR, 0666);
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

int32_t init_test_framework(void)
{
	int32_t rtrn = 0;
    char *home = NULL;

    rtrn = get_home(&home);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't get user's home path\n");
    	return (-1);
    }

    rtrn = asprintf(&shared_path, "%s/shared_mapping", home);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't create shared map path\n");
    	free(home);
    	return (-1);
    }

    mem_free(home);

	return (0);
}

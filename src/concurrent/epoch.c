/*
 * Copyright (c) 2016, Harrison Bowden, Minneapolis, MN
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
 */

#include "epoch.h"
#include "io/io.h"
#include "memory/memory.h"

static int32_t section_buf_size = 8;

struct thread_ctx
{
	epoch_record *record;

	epoch_section **section;

	uint32_t section_count;
};

struct thread_ctx *init_thread(epoch_ctx *epoch)
{
	struct thread_ctx *thread = NULL;

    /* Allocate the thread context. */
	thread = mem_alloc(sizeof(struct thread_ctx));
	if(thread == NULL)
	{
		output(ERROR, "Thread context allocation failed\n");
		return (NULL);
	}

    /* Allocate an array of epoch section pointers. */
	thread->section = mem_alloc(section_buf_size * sizeof(epoch_section *));
	if(thread->section == NULL)
	{
		output(ERROR, "Epoch section allocation failed\n");
		return (NULL);
	}

	thread->record = mem_alloc(sizeof(epoch_record));
	if(thread->record == NULL)
	{
		output(ERROR, "Epoch record allocation failed\n");
		return (NULL);
	}

    /* Initialize the epoch record. */
	epoch_register(epoch, thread->record);

    /* Set section count to zero. */
	thread->section_count = 0;

	return (thread);
}

epoch_record *get_record(struct thread_ctx *thread)
{
	return (thread->record);
}

void epoch_start(struct thread_ctx *thread)
{
	return;
}

void epoch_stop(struct thread_ctx *thread)
{
	return;
}
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

#include "ck_epoch.h"
#include "io/io.h"
#include "memory/memory.h"

struct thread_ctx;

typedef ck_epoch_t epoch_ctx;

typedef ck_epoch_record_t epoch_record;

typedef ck_epoch_section_t epoch_section;

/**
 *    Function like macro for initializing a epoch object.
 *    @param epoch A pointer to the epoch object to initialize.
 */
#define epoch_init(epoch) ck_epoch_init(epoch)

/**
 *    Function like macro for registering a threads epoch_record
 *    with the global epoch.
 *    @param epoch A pointer to the epoch object to register the thread with.
 *    @param record A pointer to the record to register.
 */
#define epoch_register(epoch, record) ck_epoch_register(epoch, record)

/**
 *    Function like macro for unregistering a threads epoch_record
 *    with the global epoch.
 *    @param record A pointer to the record to unregister.
 */
#define epoch_unregister(record) ck_epoch_unregister(record)

/**
 *
 *
 */
#define epoch_begin(record, section) ck_epoch_begin(record, section)

/**
 *
 *
 */
#define epoch_end(record, section) ck_epoch_end(record, section)

extern struct thread_ctx *init_thread(epoch_ctx *, struct memory_allocator *, struct output_writter *);
extern epoch_record *get_record(struct thread_ctx *thread);

extern int32_t epoch_start(struct thread_ctx *thread);
extern void epoch_stop(struct thread_ctx *thread);

extern void stop_all_sections(struct thread_ctx *thread);

extern void clean_thread(struct thread_ctx **thread);

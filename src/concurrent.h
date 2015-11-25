

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

#ifndef CONCURRENT_H
#define CONCURRENT_H

#include "ck_queue.h"
#include "ck_spinlock.h"
#include "stdatomic.h"

#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>

#define LIST_ENTRY(x) CK_LIST_ENTRY(x) list_entry
#define SLIST_ENTRY(x) CK_SLIST_ENTRY(x) list_entry

#define LIST_HEAD(name,type) 
#define SLIST_HEAD(name,type) CK_SLIST_HEAD(name, type) name

#define SPINLOCK_INITIALIZER CK_SPINLOCK_INITIALIZER
#define SLIST_HEAD_INITIALIZER CK_SLIST_HEAD_INITIALIZER

typedef ck_spinlock_t spinlock_t;

#ifdef MAC_OSX

#include <mach/port.h>
#include <mach/mach.h>

/* Message ports are really mach ports on Mac OSX systems. */
typedef mach_port_t msg_port_t;

#endif

/* CAS loop for swapping atomic int32 values. */ 
extern void cas_loop_int32(atomic_int_fast32_t *target, int32_t value);

/* CAS loop for swapping atomic uint32 values. */ 
extern void cas_loop_uint32(atomic_uint_fast32_t *target, uint32_t value);

/* Simple wrapper function so we can wait on atomic pid values.  */
extern int32_t wait_on(atomic_int_fast32_t *pid, int32_t *status);

/* Message IPC sending function. */
extern int32_t msg_send(msg_port_t send_port, msg_port_t remote_port, void *msg);

/*  */
extern int32_t msg_recv(msg_port_t recv_port, void *buffer);

extern msg_port_t init_msg_port(void);

#endif

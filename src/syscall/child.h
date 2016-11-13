/**
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
 **/

#ifndef NX_SYSCALL_CHILD_H
#define NX_SYSCALL_CHILD_H

#include "io/io.h"
#include "memory/memory.h"

enum child_state {EMPTY, INITIALIZING};

struct syscall_child
{
    pid_t pid;
    int32_t (*start)(void);
    int32_t (*stop)(void);
};

/* This child state state object is used for sharing
  information about running syscall child processes.
  This object will be allocated as shared memory so
  multiple processes can view it. */
struct children_state
{
    /* The number of children processes currently running. */
    uint32_t running_children;

    /* Counter for number of syscall test that have been completed. */
    uint32_t test_counter;

    /* The total number of children processes to run. */
    const uint32_t total_children;

    /* An array of child context structures. These structures track variables
       local to the child process. */
    struct syscall_child **children;
};

/**
 * This function creates a syscall child process. Only call this function from
 * the main program thread, or at least not from any child processes.
 * @param Pass a children state object so it can be updated.
 * @return A child object on success and NULL on failure.
 */
extern struct syscall_child *create_syscall_child(struct children_state *);

extern struct children_state *create_children_state(struct memory_allocator *allocator,
                                                    struct output_writter *,
                                                    uint32_t);

#endif

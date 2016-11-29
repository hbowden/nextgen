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
#include <setjmp.h>

enum child_state {EMPTY, INITIALIZING};

struct children_state;

struct syscall_child
{
    pid_t pid;
    int32_t (*start)(struct syscall_child *, struct output_writter *);
    int32_t (*stop)(void);
    int32_t ret_value;
    int32_t had_error;
    int32_t sig_num;
    int32_t did_jump;
    jmp_buf return_jump;
};

/* This children state object is used for sharing
  information about running syscall child processes.
  This object will be allocated as shared memory so
  multiple processes can access it. */
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
 * @return A child object on success and NULL on failure.
 */
extern struct syscall_child *create_syscall_child(void);

/**
 * Use this function to create a children state object.
 * This state object is used to track info about running syscall children,
 * and is required to create syscall children.
 * @param Pass a memory_allocator so create_children_state() can allocate the state object.
 * @param Pass a output_writter so create_children_state() can log poetential errors.
 * @return A children_state object/struct is returned on success and NULL on error.
 */
extern struct children_state *create_children_state(struct memory_allocator *allocator,
                                                    struct output_writter *,
                                                    uint32_t);
/**
 * Return the syscall child object/struct that coresponds with the pid passed.
 * @param The pid of the child process who's child object you wan't.
 * @return A syscall_child object/struct on success and NULL when no
 * child is found or there is an error.
 */
extern struct syscall_child *get_child_with_pid(pid_t pid);

/**
 * Kill's all syscall children processes currently running.
 */
extern void kill_all_children(void);

/**
 * This function will jump to the last saved point of execution.
 * @param The syscall_child object of the child process we want to jump to.
 */
extern void jump(struct syscall_child *child);

/**
 * Sets the children_state object to be used by create_syscall_child(),
 * get_child_with_pid() and other functions.
 * @param A pointer to a children state object that was created with create_children_state().
 * @return Does not fail, so no return value.
 */
extern void set_children_state(struct children_state *);

/**
 * A thread safe function for setting the pid of a syscall child struct/object.
 * @param The syscall object/struct to have it's pid updated.
 * @param The pid value to update the syscall child with.
 * @return Does not fail, so no return value.
 */
extern void set_child_pid(struct syscall_child *child, int32_t pid);

/**
 * Sets the had_error flag in a sysycall_child object/struct in a thread safe manner.
 * @param The syscall_child object/struct to update.
 * @param The value to set the syscall_child had_error flag.
 * @return Does not fail, so no return value.
 */
extern void set_had_error(struct syscall_child *child, int32_t val);

/**
 * Sets the ret_value flag in a sysycall_child object/struct in a thread safe manner.
 * @param The syscall_child object/struct to update.
 * @param The value to set the syscall_child ret_value flag.
 * @return Does not fail, so no return value.
 */
extern void set_ret_value(struct syscall_child *child, int32_t val);

/**
 * Sets the sig_num flag in a sysycall_child object/struct in a thread safe manner.
 * @param The syscall_child object/struct to update.
 * @param The value to set the syscall_child sig_num flag.
 * @return Does not fail, so no return value.
 */
extern void set_sig_num(struct syscall_child *child, int32_t num);

/**
 * Sets the did_jump flag in a sysycall_child object/struct in a thread safe manner.
 * @param The syscall_child object/struct to update.
 * @param The value to set the syscall_child did_jump flag.
 * @return Does not fail, so no return value.
 */
extern void set_did_jump(struct syscall_child *child, int32_t val);

/**
 * Sets the arg_size flag in a sysycall_child object/struct in a thread safe manner.
 * @param The syscall_child object/struct to update.
 * @param The value to set the syscall_child arg_size flag.
 * @return Does not fail, so no return value.
 */
extern void set_arg_size(struct syscall_child *child, uint64_t size);

#endif

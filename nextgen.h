

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

#ifndef NEXTGEN_H
#define NEXTGEN_H

#include "child.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>

enum child_state {EMPTY};

#define FALSE 0
#define TRUE 1

enum fuzz_mode { MODE_FILE, MODE_SYSCALL, MODE_NETWORK};

struct executable_context
{
    pid_t pid;

    /* The virtual memory address found in the execuatble header. */
    unsigned long main_start_address;

    /* The virtual memory offset of where the executable begins. */
    unsigned long start_offset;

    unsigned long end_offset;

    unsigned long number_of_branchs;

    /* The arguments to pass to the target executable. */
    char *args[];
};

struct shared_map
{
    /* This tells us what fuzzing mode to execute. */
    enum fuzz_mode mode;

    char *path_to_in_dir;
    char *path_to_out_dir;
    char *path_to_exec;

    /* Here is where we keep data about the executable we are fuzzing. */
    struct executable_context *exec_ctx;

    /* The pids of our helper processes. */
    pid_t reaper_pid;
    pid_t runloop_pid;

    /* These variables let us know how to derive our random numbers. */
    char *crypto_method;
    bool crypto_flag;

    /* An atomic value used to tell the processes whether to run or not. */
    atomic_bool stop;

    /* If this mode is TRUE then we don't use the binary feedback and genetic algorithm. */
    bool dumb_mode;

    unsigned int running_children;
    unsigned int number_of_children;
    
    struct child_ctx **children;
};

extern struct shared_map *map;

#endif
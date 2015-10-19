

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
#include "plugin.h"
#include "concurrent.h"
#include "syscall_table.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dtrace.h>

#include "stdatomic.h"

enum child_state {EMPTY};

#ifdef FREEBSD

enum local_bool { FALSE, TRUE };

#endif

enum fuzz_mode { MODE_FILE, MODE_SYSCALL, MODE_NETWORK };

struct executable_context
{
    atomic_int_fast32_t pid;

    char *path_to_exec;

    char **address_index;

    /* The virtual memory address found in the execuatble header. */
    unsigned long main_start_address;

    /* The virtual memory offset of where the executable begins. */
    unsigned long start_offset;
    
    unsigned long end_offset;

    unsigned long number_of_branchs;

    /* The arguments to pass to the target executable. */
    char *args[];
};

/* This struct is mapped as shared anonymous memory and is used to communicate between
the various process and threads created by nextgen. */
struct shared_map
{
    /* This tells us what fuzzing mode to execute. */
    enum fuzz_mode mode;

    /* The input and output directory paths. */
    char *path_to_in_dir;
    char *path_to_out_dir;

    /* Here is where we keep data about the executable we are fuzzing. */
    struct executable_context *exec_ctx;

    /* Index of file paths read from the in directory. */
    char **file_index;

    /* Number of files in index. */
    unsigned int file_count;

    /* Plugin context index. */
    struct plugin_ctx **plugins;

    struct work_queue *queue;

    /* Number of plugins in plugin index. */
    unsigned int plugin_count;

    /* As the name implies this is a handle used for dtrace. */
    dtrace_hdl_t *dtrace_handle;

    /* The pids of our helper processes. */
    atomic_int_fast32_t reaper_pid;
    atomic_int_fast32_t runloop_pid;
    atomic_int_fast32_t socket_server_pid;
    atomic_int_fast32_t god_pid;

    atomic_int_fast64_t test_counter;

    /* These variables let us know how to derive our random numbers. */
    char *crypto_method;
    bool crypto_flag;

    /* An atomic value used to tell the various processes whether to run or not. */
    atomic_int_fast32_t stop;

    /* If this mode is FALSE then we don't use the binary feedback and genetic algorithm. */
    bool smart_mode;

    /* The port that the ipv4 socket server is on. The ipv6 port is ipv4 + 1. */
    unsigned int socket_server_port;

    /* The max number of children to create. */
    unsigned int number_of_children;
};

extern struct shared_map *map;

#endif

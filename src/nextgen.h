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

#ifndef NEXTGEN_H
#define NEXTGEN_H

#include "platform.h"
#include "crypto/crypto.h"
#include "concurrent/concurrent.h"
#include "syscall/syscall_table.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "stdatomic.h"

enum fuzz_mode { MODE_FILE, MODE_SYSCALL, MODE_NETWORK };

struct parser_ctx
{
    enum fuzz_mode mode;
    char *path_to_exec;
    char *path_to_in_dir;
    char *path_to_out_dir;
    enum crypto_method method;
    char *args;
    int32_t smart_mode;
};

/* This struct is mapped as shared anonymous memory and is used to communicate between
the various process and threads created by nextgen. */
struct shared_map
{
    /* This tells us what fuzzing mode to execute. */
    enum fuzz_mode mode;

    /* The method with which to derive the random numbers. */
    enum crypto_method method;

    /* The input and output directory paths. */
    char *path_to_in_dir;
    char *path_to_out_dir;

    /* Path to target executable. */
    char *exec_path;

    /* The genetic algorithm's message port. */
    msg_port_t msg_port;

    /* The PID's of our helper processes. */
    atomic_int_fast32_t god_pid;
    atomic_int_fast32_t target_pid;
    atomic_int_fast32_t reaper_pid;
    atomic_int_fast32_t runloop_pid;
    atomic_int_fast32_t socket_server_pid;

    /* Counter for the number of fuzz test perform. */
    atomic_uint_fast64_t test_counter;

    /* An atomic value used to tell the various processes whether to run or not. */
    atomic_int_fast32_t stop;

    /* If this mode is FALSE then we don't use the binary feedback and genetic algorithm. */
    bool smart_mode;

    /* The port that the ipv4 socket server is on. The ipv6 port is ipv4 + 1. */
    uint32_t socket_server_port;
};

extern struct shared_map *map;

extern int32_t init_shared_mapping(struct shared_map **mapping, struct parser_ctx *ctx);

extern void clean_shared_mapping(void);

extern struct parser_ctx *parse_cmd_line(int argc, char *argv[]);

#endif

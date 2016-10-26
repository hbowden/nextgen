

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

#ifndef ARG_TYPES_H
#define ARG_TYPES_H

#include <stdint.h>
#include "io/io.h"
#include "utils/utils.h"

enum arg_type {

    FILE_DESC,
    VOID_BUF,
    SIZE,
    FILE_PATH,
    OPEN_FLAG,
    MODE,
    STAT_FS,
    STAT_FLAG,
    INT,
    RUSAGE,
    PID,
    WAIT_OPTION,
    SOCKET,
    WHENCE,
    OFFSET,
    MOUNT_TYPE,
    DIR_PATH,
    MOUNT_FLAG,
    UNMOUNT_FLAG,
    RECV_FLAG,
    REQUEST,
    MOUNT_PATH,
    DEV,
    PTRACE_REQUEST,
    MESSAGE,
    SEND_FLAG,
    SOCKADDR,
    SOCKLEN,
    AMODE,
    CHFLAGS
};

struct arg_context
{
	  const char *name;
	  enum arg_type type;
	  enum yes_no should_free;
	  int32_t log_type;
    const char padding[4];
};

extern struct arg_context *get_arg_context(enum arg_type type, struct output_writter *);

extern struct arg_context file_desc_ctx;

extern struct arg_context void_buf_ctx;

extern struct arg_context size_ctx;

extern struct arg_context file_path_ctx;

extern struct arg_context open_flag_ctx;

extern struct arg_context mode_ctx;

extern struct arg_context stat_fs_ctx;

extern struct arg_context stat_flag_ctx;

extern struct arg_context int_ctx;

extern struct arg_context rusage_ctx;

extern struct arg_context pid_ctx;

extern struct arg_context wait_option_ctx;

extern struct arg_context socket_ctx;

extern struct arg_context whence_ctx;

extern struct arg_context offset_ctx;

extern struct arg_context mount_type_ctx;

extern struct arg_context dir_path_ctx;

extern struct arg_context mount_flag_ctx;

extern struct arg_context unmount_flag_ctx;

extern struct arg_context recv_flag_ctx;

extern struct arg_context request_ctx;

extern struct arg_context mount_path_ctx;

extern struct arg_context dev_ctx;

extern struct arg_context message_ctx;

extern struct arg_context recv_flags_ctx;

extern struct arg_context send_flags_ctx;

extern struct arg_context sockaddr_ctx;

extern struct arg_context socklen_ctx;

extern struct arg_context amode_ctx;

extern struct arg_context chflags_ctx;

#endif

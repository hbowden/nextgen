

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

#ifndef ARG_TYPES_H
#define ARG_TYPES_H

#include "private.h"
#include "utils.h"
#include "log.h"

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
    DEV
};

struct arg_context
{
	const char *name;
	enum arg_type type;
	enum yes_no should_free;
	enum logging_type log_type;

};

private extern struct arg_context file_desc_ctx;

private extern struct arg_context void_buf_ctx;

private extern struct arg_context size_ctx;

private extern struct arg_context file_path_ctx;

private extern struct arg_context open_flag_ctx;

private extern struct arg_context mode_ctx;

private extern struct arg_context stat_fs_ctx;

private extern struct arg_context stat_flag_ctx;

private extern struct arg_context int_ctx;

private extern struct arg_context rusage_ctx;

private extern struct arg_context pid_ctx;

private extern struct arg_context wait_option_ctx;

private extern struct arg_context socket_ctx;

private extern struct arg_context whence_ctx;

private extern struct arg_context offset_ctx;

private extern struct arg_context mount_type_ctx;

private extern struct arg_context dir_path_ctx;

private extern struct arg_context mount_flag_ctx;

private extern struct arg_context unmount_flag_ctx;

private extern struct arg_context recv_flag_ctx;

private extern struct arg_context request_ctx;

private extern struct arg_context mount_path_ctx;

private extern struct arg_context dev_ctx;

#endif

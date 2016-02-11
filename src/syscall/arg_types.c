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

#include "arg_types.h"
#include "log/log.h"
#include "io/io.h"

struct arg_context file_desc_ctx = {

    .name = "FILE_DESC",
    .type = FILE_DESC,
    .should_free = NO,
    .log_type = NUMBER

};

struct arg_context void_buf_ctx = {

	.name = "VOID_BUF",
    .type = VOID_BUF,
    .should_free = YES,
    .log_type = POINTER

};

struct arg_context size_ctx = {

	.name = "SIZE",
    .type = SIZE,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context file_path_ctx = {

	.name = "FILE_PATH",
    .type = FILE_PATH,
    .should_free = NO,
    .log_type = PATH

};

struct arg_context open_flag_ctx = {

	.name = "OPEN_FLAG",
    .type = OPEN_FLAG,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context mode_ctx = {

	.name = "MODE",
    .type = MODE,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context stat_fs_ctx = {

	.name = "STAT_FS",
    .type = STAT_FS,
    .should_free = YES,
    .log_type = POINTER

};

struct arg_context stat_flag_ctx = {

	.name = "STAT_FLAG",
    .type = STAT_FLAG,
    .should_free = YES,
    .log_type = NUMBER

}; 

struct arg_context int_ctx = {

	.name = "INT",
    .type = INT,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context rusage_ctx = {

	.name = "RUSAGE",
    .type = RUSAGE,
    .should_free = YES,
    .log_type = POINTER

};

struct arg_context pid_ctx = {

	.name = "PID",
    .type = PID,
    .should_free = YES,
    .log_type = POINTER

};

struct arg_context wait_option_ctx = {

	.name = "WAIT_OPTION",
    .type = WAIT_OPTION,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context socket_ctx = {

	.name = "SOCKET",
    .type = SOCKET,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context whence_ctx = {

	.name = "WHENCE",
    .type = WHENCE,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context offset_ctx = {

	.name = "OFFSET",
    .type = OFFSET,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context mount_type_ctx = {

	.name = "MOUNT_TYPE",
    .type = MOUNT_TYPE,
    .should_free = YES,
    .log_type = POINTER

};

struct arg_context dir_path_ctx = {

	.name = "DIR_PATH",
    .type = DIR_PATH,
    .should_free = NO,
    .log_type = POINTER

};

struct arg_context mount_flag_ctx = {

	.name = "MOUNT_FLAG",
    .type = MOUNT_FLAG,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context unmount_flag_ctx = {

	.name = "UNMOUNT_FLAG",
    .type = UNMOUNT_FLAG,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context recv_flag_ctx = {

	.name = "RECV_FLAG",
    .type = RECV_FLAG,
    .should_free = YES,
    .log_type = NUMBER

};
    
struct arg_context request_ctx = {

	.name = "REQUEST",
    .type = REQUEST,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context mount_path_ctx = {

	.name = "MOUNT_PATH",
    .type = MOUNT_PATH,
    .should_free = NO,
    .log_type = PATH

};

struct arg_context dev_ctx = {

    .name = "dev",
    .type = DEV,
    .should_free = YES,
    .log_type = NUMBER

};

struct arg_context *get_arg_context(enum arg_type type)
{
    switch((int)type)
    {
        case FILE_DESC: return &file_desc_ctx;

        case VOID_BUF: return &void_buf_ctx;
            
        case SIZE: return &size_ctx;

        case FILE_PATH: return &file_path_ctx;

        case OPEN_FLAG: return &open_flag_ctx;
           
        case MODE: return &mode_ctx;

        case STAT_FS: return &stat_fs_ctx;

        case STAT_FLAG: return &stat_flag_ctx;

        case INT: return &int_ctx;
       
        case RUSAGE: return &rusage_ctx;
         
        case PID: return &pid_ctx;
          
        case WAIT_OPTION: return &wait_option_ctx;
         
        case SOCKET: return &socket_ctx;
           
        case WHENCE: return &whence_ctx;
           
        case OFFSET: return &offset_ctx;
        
        case MOUNT_TYPE: return &mount_type_ctx;
         
        case DIR_PATH: return &dir_path_ctx;
          
        case MOUNT_FLAG: return &mount_flag_ctx;

        case UNMOUNT_FLAG: return &unmount_flag_ctx;

        case RECV_FLAG: return &recv_flag_ctx;
         
        case REQUEST: return &request_ctx;

        case MOUNT_PATH: return &mount_path_ctx;

        case DEV: return &dev_ctx;

        default:
            output(ERROR, "Unlisted arg type\n");
            return NULL;
    }
}





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

#include "syscall_list.h"

struct syscall_entry entry_recvmsg = {
    .syscall_name = "recvmsg",
    .syscall_symbol = SYS_recvmsg,
    .total_args = 3,
    .status = ON,
    .requires_root = NX_NO,
    .need_alarm = NX_NO,
    .id = VPV_ID,

    .arg_type_array[FIRST_ARG] = SOCKET,
    .get_arg_array[FIRST_ARG] = &generate_socket,

    .arg_type_array[SECOND_ARG] = MESSAGE,
    .get_arg_array[SECOND_ARG] = &generate_message,

    .arg_type_array[THIRD_ARG] = RECV_FLAG,
    .get_arg_array[THIRD_ARG] = &generate_recv_flags,
};

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

#ifndef LOG_H
#define LOG_H

#include <stdint.h>
#include "syscall/arg_types.h"

enum logging_type { POINTER, PATH, NUMBER };

extern int32_t log_arguments(uint32_t total_args,
	                         const char *syscall_name,
	                         uint64_t **arg_value_array,
	                         struct arg_context **arg_context_array,
												   struct memory_allocator *allocator);

extern int32_t write_arguments_to_log(uint32_t total_args,
                                      uint64_t **arg_value_array,
                                      uint32_t syscall_number);

extern int32_t log_results(int32_t had_error, int32_t ret_value, char *err_value);

extern int32_t log_file(char *file_path, char *file_extension);

extern int32_t setup_log_module(char *path, uint32_t syscall_max);

#endif

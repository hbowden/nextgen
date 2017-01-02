
/**
 * Copyright (c) 2017, Harrison Bowden, Minneapolis, MN
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



 /* WARNING, THIS FILE WAS MACHINE GENERATED. DO NOT MODIFY THIS FILE DIRECTLY
   BECAUSE YOUR CHANGES WILL BE OVERWRITTEN WHEN THIS FILE IS GENERATED AGAIN.
   IF YOU WAN'T TO MODIFY THIS FILE YOU SHOULD BE MODIFYING THE GENERATOR IT'S SELF
   AND REGENERATE THE SYSCALL ENTRY FILES. */


#include "syscall_list.h"

struct syscall_entry entry_searchfs = {
    .syscall_name = "searchfs",
    .syscall_number = 225,
    .total_args = 6,
    .return_type = "int",
    .status = ON,
  
    .arg_type_array[FIRST_ARG] = ADDRESS,
    .get_arg_array[FIRST_ARG] = &generate_ptr,
  
    .arg_type_array[SECOND_ARG] = ADDRESS,
    .get_arg_array[SECOND_ARG] = &generate_ptr,
  
    .arg_type_array[THIRD_ARG] = ADDRESS,
    .get_arg_array[THIRD_ARG] = &generate_ptr,
  
    .arg_type_array[FOURTH_ARG] = INT,
    .get_arg_array[FOURTH_ARG] = &generate_int,
  
    .arg_type_array[FIFTH_ARG] = INT,
    .get_arg_array[FIFTH_ARG] = &generate_int,
  
    .arg_type_array[SIXTH_ARG] = ADDRESS,
    .get_arg_array[SIXTH_ARG] = &generate_ptr,
  
};

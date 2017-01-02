

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

#ifndef ENTRY_H
#define ENTRY_H

//#include "runtime/platform.h"
#include "syscall.h"
#include "child.h"
#include "arg_types.h"

#include <stdbool.h>

enum test_id { V_ID, P_ID, VP_ID, VPV_ID, PV_ID, PVV_ID,
               PP_ID, VPVVPV_ID, VPVP_ID,
               VVPV_ID };

enum syscall_status { ON, OFF };

enum argnums { FIRST_ARG, SECOND_ARG, THIRD_ARG, FOURTH_ARG, FIFTH_ARG, SIXTH_ARG, SEVENTH_ARG, EIGTH_ARG};

struct syscall_entry
{
  const char *syscall_name;
  const int32_t syscall_symbol;

  int32_t status;
  const int32_t need_alarm;
  const int32_t requires_root;

  const char padding[2];
  int32_t arg_type_array[9];

  const uint32_t total_args;
  const uint32_t entry_number;

  const char padding2[8];

  struct arg_context *arg_context_array[9];

  int32_t (*get_arg_array[9])(uint64_t **);

  int32_t (*test_syscall)(int32_t, uint64_t **);

  enum test_id id;

  uint32_t syscall_number;

  const char *return_type;
};

#endif

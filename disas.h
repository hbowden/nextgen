

#ifndef DISAS_H
#define DISAS_H

#include "private.h"

#include <stdint.h>

private extern int disas_executable(char *path);

private extern int get_load_address(char *path, uint32_t *start_address);

#endif
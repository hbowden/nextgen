

#ifndef UTILS_H
#define UTILS_H

#include "private.h"
#include <stdarg.h>
#include <unistd.h>

enum out_type { ERROR, STD };

/* This function as the name implies gets the file size. */
private extern int get_file_size(int fd, off_t *size);

/* map_file_in() takes the file descriptor pointed to by fd and mmaps the file to buf
and puts the files size in size.  */
private extern int map_file_in(int *fd, char **buf, off_t *size);

/* This function maps a buffer of size to a file path.  */
private extern int map_file_out(char *path, char *buf, off_t *size);

/* This function replaces printf and perror in the code so we can aggregate output to one point. */
private extern int output(enum out_type type, const char *format, ...);

private extern int get_core_count(unsigned int *core_count);

#endif
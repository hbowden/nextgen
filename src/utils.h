

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

#ifndef UTILS_H
#define UTILS_H

#include "private.h"
#include "arg_types.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <ck_queue.h>

#include "stdatomic.h"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLD_BLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLD_RED     "\033[1m\033[31m"      /* Bold Red */
#define BOLD_GREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLD_YELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLD_BLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLD_MAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLD_CYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLD_WHITE   "\033[1m\033[37m"      /* Bold White */

enum yes_no { YES, NO };

struct list_data
{
    struct timeval create_time;
    int arg_type;
    int socket_or_fd;
    int node_number;
    char *path;
    pid_t pid;
};

struct list_node
{
    struct list_data *data;

    CK_LIST_ENTRY(list_node) list_entry;

};

static inline void cleanup_buf(char **buf)
{
	if(*buf == NULL)
	{
		return;
	}

	free(*buf);
}

static inline void close_fd(int *fd)
{
	close(*fd);
}

/* Use this attribute for variables that we want to automatically cleanup. */
#define auto_clean __attribute__((cleanup (cleanup_buf)))

#define auto_close __attribute__((cleanup (close_fd)))

/* The enum used to tell output how to output the message. */
enum out_type { ERROR, STD };

enum csp { SET, UNSET };

enum name_type { DIR_NAME, FILE_NAME };

/* This function as the name implies gets the file size. */
private extern int get_file_size(int fd, off_t *size);

/* map_file_in() takes the file descriptor pointed to by fd and mmaps the file to buf
and puts the files size in size.  */
private extern int map_file_in(int fd, char **buf, off_t *size);

/* This function maps a buffer of size to a file path.  */
private extern int map_file_out(char *path, char *buf, off_t size);

private extern int copy_file_to(char *copy_path, char *out_path);

/* This function replaces printf and perror in the code so we can aggregate output to one point. */
private extern void output(enum out_type type, const char *format, ...);

/* Get the core count of the system we are on. This will include virtual cores on hyperthreaded systems. */
private extern int get_core_count(unsigned int *core_count);

/* Can be used to create random file and directory paths. */
private extern int generate_name(char **name, char *extension, enum name_type type);

/* Simple wrapper function so we can wait on atomic pid values.  */
private extern int wait_on(atomic_int_fast32_t *pid, int *status);

/* Grabs the path to the user's home directory\n*/
private extern int get_home(char **home);

/* Creates a shared memory map. */
private extern int create_shared(void **pointer, int size);

/* CAS loop for swapping atomic bool values. */ 
private extern int compare_and_swap_bool(atomic_bool *target, int value);

/* CAS loop for swapping atomic int32 values. */ 
private extern int compare_and_swap_int32(atomic_int_fast32_t *target, int value);

/* CAS loop for swapping atomic uint_32_t values. */
private extern int compare_and_swap_uint32(atomic_uint_fast32_t *target, unsigned int value);

/* CAS loop for swapping atomic int_64_t values. */
private extern int compare_and_swap_int64(atomic_int_fast64_t *target, long value);

/* CAS loop for swapping atomic uint_64_t values. */
private extern int compare_and_swap_uint64(atomic_uint_fast64_t *target, unsigned long value);

#endif
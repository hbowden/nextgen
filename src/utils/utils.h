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

#ifndef UTILS_H
#define UTILS_H

#include "io/io.h"
#include "deprecate.h"
#include "crypto/crypto.h"

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>

enum yes_no { NX_YES, NX_NO };

enum csp { SET, UNSET };

enum name_type { DIR_NAME, FILE_NAME };

int32_t run_syscall(int32_t number, ...);

#ifndef FREEBSD

extern void *reallocarray(void *optr, size_t nmemb, size_t size);

#endif

/**
 * This function as the name implies gets the file size.
 * @param fd A file descriptor to get the size of.
 * @param size A pointer to a unsigned 64 bit integer where the file size will be placed.
 * @param output The output writter to write error messages to.
 * @return A zero on success or negative one on failure.
 */
extern int32_t get_file_size(int32_t, uint64_t *, struct output_writter *);

/**
 * This function will memory map a file into memory.
 * @param fd The file descriptor of the file to map into memory.
 * @param buf The buffer were the file will be mapped into memory. This buffer will be allocated by map_file_in().
 * @param size The size of the file and memory buffer, this param will be set by map_file_in().
 * @param perm The permissions to set on the memory buffer, buf.
 * @param output The output writter where error messages will be written.
 * @return Zero on success, negative one in an error.
 */
extern int32_t map_file_in(int32_t, char **, uint64_t *, int32_t, struct output_writter *);

/* This function maps a buffer of size to a file path.  */
extern int32_t map_file_out(char *path, char *buf, uint64_t size, struct output_writter *output);

/* Copy a file from one file path to another. */
extern int32_t copy_file_to(char *src, char *dst, struct output_writter *output);

/* Get file extension */
extern int32_t get_extension(char *path, char **extension);

/* Get the core count of the system we are on. This will include virtual cores on hyperthreaded systems. */
extern int32_t get_core_count(uint32_t *core_count);

/* Grabs the path to the user's home directory. */
extern int32_t get_home(char **home);

/* Convert the ascii string to a binary string upto len bytes length. */
extern int32_t ascii_to_binary(char *input, char **out, uint64_t input_len, uint64_t *out_len);

/* Convert binary string to a ascii string. */
extern int32_t binary_to_ascii(char *input, char **out, uint64_t input_len, uint64_t *out_len);

/* This function will return zero if the process calling it has root privileges. */
extern int32_t check_root(void);

/**
 * Create a random file.
 * @param root The directory path at which to create the new random file.
 * @param ext The file extension to give the new random file.
 * @param path On success an allocated buffer with the full file path will be placed here.
 * @param size The size of the newly created file will be set here on success.
 * @param random The random number generator interface used for creating the new file.
 * @param allocator The memory allocator interface to use for allocating memory.
 * @param output The output writter to write error messages to.
 * @return Zero on success and negative one when an error occurs.
 */
extern int32_t create_random_file(char *,
                                  char *,
                                  char **,
                                  uint64_t *,
                                  struct random_generator *,
                                  struct memory_allocator *,
                                  struct output_writter *);

/* Create a random directory at path root the path created
 will be put in the buffer path. */
extern int32_t create_random_directory(char *, char **, struct output_writter *);

/* Drop privileges.  */
extern int32_t drop_privileges(void);

/* Delete a directory and all it's contents. */
extern int32_t delete_directory(char *path);

/* Delete the contents of the directory but not the directory it's self. */
extern int32_t delete_dir_contents(char *dir);

/* Count files directory. */
extern int32_t count_files_directory(uint32_t *count, char *dir);

/*
 * Call to set the name of the process we are currently running in.
 * @param name The name to set the currently running processe's name to.
 * @param output An output_writter object/struct for logging purposes.
 */
extern void set_process_name(char *, struct output_writter *);

extern int32_t generate_file_name(char **, char *, struct output_writter *, struct random_generator *);

extern int32_t generate_directory_name(char **, struct output_writter *);

/* End of header. */
#endif

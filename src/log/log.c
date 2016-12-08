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

#ifdef LINUX

/* We need to define _GNU_SOURCE to use
 asprintf on Linux. We also need to place
 _GNU_SOURCE at the top of the file before
 any other includes for it to work properly. */
#define _GNU_SOURCE

#endif

#include "log.h"
#include "io/io.h"
#include "utils/colors.h"
#include "utils/autofree.h"
#include "memory/memory.h"
#include "runtime/platform.h"
#include "utils/utils.h"

#include <sqlite3.h>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static struct memory_allocator *allocator;
static struct output_writter *output;

/* Sqlite3 database connection object. */
//static sqlite3 *db;

int32_t log_file(char *file_path, char *file_extension)
{
    (void)file_path;
    (void)file_extension;

    /*
    int32_t rtrn = 0;
    char *out_path auto_free = NULL;

    rtrn = asprintf(&out_path, "%s/last_file_run.%s", out_dir_path,
                    file_extension);
    if(rtrn < 0)
    {
        printf("Can't create out path: %s\n", strerror(errno));
        return (-1);
    }

    rtrn = copy_file_to(file_path, out_path);
    if(rtrn < 0)
    {
        printf("Can't copy file to the out directory\n");
        return (-1);
    }*/

    return (0);
}

// static int32_t create_log_file(char *path)
// {
//     int32_t rtrn = 0;
//
//     if(path == NULL)
//         return (-1);
//
//     rtrn = sqlite3_open(path, &db);
//     if(rtrn != SQLITE_OK)
//     {
//         printf("Can't create database: %s", strerror(errno));
//         return (-1);
//     }
//
//     return (0);
// }
//
// int32_t write_arguments_to_log(uint32_t total_args,
//                                uint64_t **arg_value_array,
//                                uint32_t syscall_number)
// {
//     /* Make sure the total argument count is in bounds. */
//     if(total_args > ARG_LIMIT)
//     {
//         printf("Total args is greater than the arg limit for this system\n");
//         return (-1);
//     }
//
//     /* Make sure the argument value array is not NULL */
//     if(arg_value_array == NULL)
//     {
//         printf("Argument value array is NULL\n");
//         return (-1);
//     }
//
//     (void)syscall_number;
//     char *err;
//     char *sql auto_free = NULL;
//
//     /* Create SQL statement. */
//     int32_t rtrn = asprintf(&sql, "INSERT INTO syscallEntry(");
//     if(rtrn < 0)
//     {
//         printf("Can't create sql statement: %s\n", strerror(errno));
//         return (-1);
//     }
//
//     /* Store the syscall argument. */
//     rtrn = sqlite3_exec(db, sql, NULL, NULL, &err);
//     if(rtrn != SQLITE_OK)
//     {
//         printf("Can't insert syscall values into db: %s\n", err);
//         return (-1);
//     }
//
//     return (0);
// }
//
// static int32_t create_out_directory(char *path)
// {
//     int32_t rtrn = 0;
//     char *crash_dir = NULL;
//
//     /* Create the output directory. */
//     rtrn = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
//     if(rtrn < 0)
//     {
//         printf("Can't create out directory\n");
//
//         /* Check if the directory exist. */
//         if(errno == EEXIST)
//         {
//             printf("Out directory already exist\n");
//             return (-1);
//         }
//
//         return (-1);
//     }
//
//     rtrn = asprintf(&crash_dir, "%s/crash_dir", path);
//     if(rtrn < 0)
//     {
//         printf("Can't create crash directory path\n");
//         return (-1);
//     }
//
//     rtrn = mkdir(crash_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
//     if(rtrn < 0)
//     {
//         printf("Can't create crash directory\n");
//         return (-1);
//     }
//
//     return (0);
// }
//
// int32_t log_results(int32_t had_error, int32_t ret_value, char *err_value)
// {
//     int32_t rtrn = 0;
//     char *out_buf auto_free = NULL;
//
//     if(err_value == NULL)
//     {
//         printf("Err value is NULL\n");
//         return (-1);
//     }
//
//     if(had_error == NX_YES)
//     {
//         rtrn = asprintf(&out_buf, " %s= %d (%s) %s\n", BOLD_RED, ret_value,
//                         err_value, RESET);
//         if(rtrn < 0)
//         {
//             printf("Can't create out message\n");
//             return -1;
//         }
//     }
//     else
//     {
//         rtrn = asprintf(&out_buf, "%s= %d %s\n", BOLD_GREEN, ret_value, RESET);
//         if(rtrn < 0)
//         {
//             printf("Can't create out message\n");
//             return -1;
//         }
//     }
//
//     printf("%s\n", out_buf);
//
//     return 0;
// }
//
// static int32_t create_arg_entry_table(void)
// {
//     char *err;
//     int32_t rtrn = 0;
//     char *sql auto_free = NULL;
//
//     /* Create SQL statement. */
//     rtrn = asprintf(&sql, "CREATE TABLE IF NOT EXISTS arg_entry(id INT NOT NULL, val BLOB NOT NULL);");
//     if(rtrn < 0)
//     {
//         printf("Can't create SQL statement: %s\n", strerror(errno));
//         return (-1);
//     }
//
//     /* Store the syscall argument. */
//     rtrn = sqlite3_exec(db, sql, NULL, NULL, &err);
//     if(rtrn != SQLITE_OK)
//     {
//         printf("Can't create argument entry table: %s\n", err);
//         sqlite3_free(err);
//         return (-1);
//     }
//
//     return (0);
// }
//
// static int32_t create_syscall_entry_table(void)
// {
//     char *err;
//     int32_t rtrn = 0;
//     char *sql auto_free = NULL;
//
//     /* Create SQL statement. */
//     rtrn = asprintf(&sql, "CREATE TABLE IF NOT EXISTS syscall_entry(id INT NOT NULL, num INT NOT NULL, name );");
//     if(rtrn < 0)
//     {
//         printf("Can't create SQL statement: %s\n", strerror(errno));
//         return (-1);
//     }
//
//     /* Store the syscall argument. */
//     rtrn = sqlite3_exec(db, sql, NULL, NULL, &err);
//     if(rtrn != SQLITE_OK)
//     {
//         printf("Can't create argument entry table: %s\n", err);
//         sqlite3_free(err);
//         return (-1);
//     }
//
//     return (0);
// }

// static int32_t setup_tables(void)
// {
//     int32_t rtrn = 0;
//
//     rtrn = create_arg_entry_table();
//     if(rtrn < 0)
//     {
//         printf("Can't create argument entry table\n");
//         return (-1);
//     }
//
//     rtrn = create_syscall_entry_table();
//     if(rtrn < 0)
//     {
//         printf("Can't create syscall entry table\n");
//         return (-1);
//     }
//
//     return (0);
// }

void inject_log_deps(struct dependency_context *ctx)
{
    uint32_t i;

    for(i = 0; i < ctx->count; i++)
    {
        switch((int32_t)ctx->array[i]->name)
        {
            case ALLOCATOR:
                allocator = (struct memory_allocator *)ctx->array[i]->interface;
                break;

            case OUTPUT:
                output = (struct output_writter *)ctx->array[i]->interface;
                break;
        }
    }
}

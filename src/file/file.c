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

#include "file.h"
#include "crypto/crypto.h"
#include "genetic/genetic.h"
#include "io/io.h"
#include "log/log.h"
#include "memory/memory.h"
#include "mutate/mutate.h"
#include "runtime/platform.h"
#include "utils/utils.h"

#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>

static int32_t *stop_ptr;

/* File context struct. */
struct file_ctx
{
    enum file_type type;

    const char padding[4];

    const char *path;

    const char *extension;

    uint64_t size;
};

static char *path_to_exec;

/* The virtual memory offset of where the executable begins. */
static uint64_t start_offset;

/* Array of file context objects from the input directory. */
static struct file_ctx **file_array;

/* Number of files in index. */
static uint32_t file_count;

static char *input_dir;

static int32_t setup;

static void ctrlc_handler(int sig)
{
    (void)sig;
    cas_loop_int32(stop_ptr, TRUE);
    return;
}

static void setup_signal_handler(void)
{
    (void)signal(SIGFPE, SIG_DFL);
    (void)signal(SIGCHLD, SIG_DFL);
    (void)signal(SIGINT, ctrlc_handler);
    return;
}

static int32_t get_file(int32_t *file, char **extension)
{
    int32_t rtrn = 0;
    uint32_t offset = 0;

    /* Pick a file array offset at random. */
    rtrn = rand_range((file_count - 1), &offset);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random number\n");
        return (-1);
    }

    /* Open the file at the random offset. */
    (*file) = open(file_array[offset]->path, O_RDWR);
    if((*file) < 0)
    {
        output(ERROR, "Can't open file: %s\n", strerror(errno));
        return (-1);
    }

    rtrn = asprintf(extension, "%s", file_array[offset]->extension);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create extension string: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

void start_file_smart_loop(void) { return; }

int32_t get_exec_path(char **exec_path)
{
    if(setup != 1)
    {
        output(ERROR, "Setup the file module first\n");
        return (-1);
    }

    (*exec_path) = path_to_exec;

    return (0);
}

void set_start_addr(uint64_t addr)
{
    start_offset = addr;

    return;
}

uint64_t get_start_addr(void) { return (start_offset); }

static struct file_ctx *init_file_ctx(char *path)
{
    int32_t rtrn = 0;
    int32_t fd auto_close = 0;
    struct file_ctx *file = NULL;

    if(path == NULL)
    {
        output(ERROR, "Path is NULL\n");
        return (NULL);
    }

    file = mem_alloc(sizeof(struct file_ctx));
    if(file == NULL)
    {
        output(ERROR, "Can't allocate file context\n");
        return (NULL);
    }

    /* Grab file extension. */
    rtrn = get_extension(path, (char **)&file->extension);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get file extension\n");
        return (NULL);
    }

    rtrn = asprintf((char **)&file->path, "%s", path);
    if(rtrn < 0)
    {
        output(ERROR, "Can't create path string\n");
        return (NULL);
    }

    /* Open file. */
    fd = open(path, O_RDONLY);
    if(fd < 0)
    {
        output(ERROR, "Can't open file\n");
        return (NULL);
    }

    rtrn = get_file_size(fd, &file->size);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get file size\n");
        return (NULL);
    }

    return (file);
}

static struct file_ctx **init_file_array(char *dir, uint32_t size)
{
    struct stat buf;
    int32_t rtrn = 0;
    DIR *directory = NULL;
    char *file_path = NULL;
    uint32_t index_counter = 0;
    struct dirent *entry = NULL;
    struct file_ctx **array = NULL;

    /* Allocate file array. */
    array = mem_alloc((size) * sizeof(struct file_ctx *));
    if(array == NULL)
    {
        output(ERROR, "Can't create file index: %s\n", strerror(errno));
        return (NULL);
    }

    /* Open the directory. */
    directory = opendir(dir);
    if(directory == NULL)
    {
        output(ERROR, "Can't open dir: %s\n", strerror(errno));
        return (NULL);
    }

    /* Walk the directory structure. */
    while((entry = readdir(directory)) != NULL)
    {
        /* Skip hidden files. */
        if(entry->d_name[0] == '.')
            continue;

        /* Create file path. */
        rtrn = asprintf(&file_path, "%s/%s", input_dir, entry->d_name);
        if(rtrn < 0)
        {
            /* Ignore error for closedir() because we are going to quit anyway. */
            closedir(directory);
            return (NULL);
        }

        /* Grab file stats. */
        rtrn = stat(file_path, &buf);
        if(rtrn < 0)
        {
            output(ERROR, "Can't get file stats: %s\n", strerror(errno));
            return (NULL);
        }

        /* Make sure the path is a file if not skip and continue. */
        if(buf.st_mode & S_IFREG)
        {
            /* Set index element to the file path we just copied. */
            array[index_counter] = init_file_ctx(file_path);
            if(array[index_counter] == NULL)
            {
                output(ERROR, "Can't init file context\n");
                return (NULL);
            }

            /* Increment the index counter. */
            index_counter++;
        }
        else
        {
            continue;
        }
    }

    if(index_counter == 0)
    {
        output(ERROR, "Input dir is empty\n");
        return (NULL);
    }

    return (array);
}

int32_t initial_fuzz_run(void)
{
    uint32_t i;

    for(i = 0; i < file_count; i++)
    {
    }

    return (0);
}

int32_t set_end_offset(uint64_t offset) 
{
    (void)offset; 
    return (0);
}

void start_main_file_loop(void)
{
    output(STD, "Starting fuzzer\n");

    /* Set up signal handler. */
    setup_signal_handler();

    /* Check if we should stop or continue running. */
    while(ck_pr_load_int(stop_ptr) == FALSE)
    {
        /* Our variables. */
        int32_t rtrn = 0;
        uint64_t file_size = 0;
        char *file_buffer = NULL;
        int32_t file auto_close = 0;
        char *file_name auto_free = NULL;
        char *file_path auto_free = NULL;
        char *file_extension auto_free = NULL;

        /* Open file from in directory. */
        rtrn = get_file(&file, &file_extension);
        if(rtrn < 0)
        {
            output(ERROR, "Can't get file\n");
            return;
        }

        /* Read file into memory. */
        rtrn = map_file_in(file, &file_buffer, &file_size, READ | WRITE);
        if(rtrn < 0)
        {
            output(ERROR, "Can't read file to memory\n");
            return;
        }

        /* Mutate the file, sometimes the file buffer grows in length
         and file_size will be updated to the new length. */
        rtrn = mutate_file(&file_buffer, file_extension, &file_size);
        if(rtrn < 0)
        {
            output(ERROR, "Can't mutate file\n");
            return;
        }

        /* Generate random file name. */
        rtrn = generate_name(&file_name, file_extension, FILE_NAME);
        if(rtrn < 0)
        {
            output(ERROR, "Can't generate random file name\n");
            return;
        }

        /* Create out path. */
        rtrn = asprintf(&file_path, "/tmp/%s", file_name);
        if(rtrn < 0)
        {
            output(ERROR, "Can't create out path\n");
            return;
        }

        /* Write the mutated file to disk. */
        rtrn = map_file_out(file_path, file_buffer, file_size);
        if(rtrn < 0)
        {
            output(ERROR, "Can't write file to disk\n");
            return;
        }

        /* Log file before we run, so if there is a kernel panic we have the
        file that caused the panic. */
        rtrn = log_file(file_path, file_extension);
        if(rtrn < 0)
        {
            output(ERROR, "Can't log file generated\n");
            return;
        }

        /* Create children process and exec the target executable and run it with
        the generated file. */
        rtrn = run_test_case(path_to_exec, file_path, file_extension);
        if(rtrn < 0)
        {
            output(ERROR, "Can't test exec with file\n");
            return;
        }

        /* Clean up our mess. */
        mem_free_shared((void **)&file_buffer, (size_t)file_size);

        rtrn = unlink(file_path);
        if(rtrn < 0)
        {
            output(ERROR, "Can't remove test case\n");
            return;
        }
    }

    output(STD, "Exiting main loop\n");

    return;
}

int32_t setup_file_module(int32_t *stop, char *exec_path,
                          char *input)
{
    int32_t rtrn = 0;

    /* Set exec path. */
    path_to_exec = exec_path;

    /* Set input directory. */
    input_dir = input;

    /* Count how many files are in the input directory. */
    rtrn = count_files_directory(&file_count, input);
    if(rtrn < 0)
    {
        output(ERROR, "Can't count files in the in directory.\n");
        return (-1);
    }

    /* Initialize the file context array. */
    file_array = init_file_array(input, file_count);
    if(file_array == NULL)
    {
        output(ERROR, "Can't init file array\n");
        return (-1);
    }

    stop_ptr = stop;

    setup = 1;

    return (0);
}

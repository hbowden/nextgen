

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

#include "file.h"
#include "mutate.h"
#include "signals.h"
#include "memory.h"
#include "genetic.h"
#include "crypto.h"
#include "nextgen.h"
#include "platform.h"
#include "io.h"

#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <dirent.h>

static atomic_int_fast32_t pid;

static char *path_to_exec;

static char **address_index;

/* The virtual memory address found in the executable header. */
static uint64_t main_start_address;

/* The virtual memory offset of where the executable begins. */
static uint64_t start_offset;
    
static uint64_t end_offset;

static uint64_t number_of_branchs;

/* The arguments to pass to the target executable. */
static char **args;

/* Index of file paths read from the in directory. */
static char **file_index;

/* Number of files in index. */
static uint32_t file_count;

static char *input_dir;

/* Here is where we keep data about the executable we are fuzzing. */
static struct executable_context *exec_ctx;

static int32_t get_file(int32_t *file, char **extension)
{
	int32_t rtrn = 0;
	uint32_t offset = 0;

    /* Pick a file index offset at random. */
    rtrn = rand_range(file_count, &offset);
	if(rtrn < 0)
	{
		output(ERROR, "Can't pick random number\n");
		return (-1);
	}

    /* Open the file at the random offset. */
	(*file) = open(file_index[offset], O_RDONLY);
	if((*file) < 0)
	{
		output(ERROR, "Can't open file: %s\n", strerror(errno));
		return (-1);
	}

    /* Get the file extension for the file we just opened. */
	rtrn = get_extension(file_index[offset], extension);
	if(rtrn < 0)
	{
		output(ERROR, "Can't get extension\n");
		return (-1);
	}
	
    return (0);
}

int get_exec_path(char **exec_path)
{
    return 0;
}

void set_start_addr(uint64_t addr)
{
    start_offset = addr;

    return;
}

uint64_t get_start_addr(void)
{
    return (start_offset);
}

static int32_t create_file_index(void)
{
    int32_t rtrn = 0;
    DIR *directory = NULL;
    char *file_path = NULL;
    uint32_t index_counter = 0;
	struct dirent *entry = NULL;

    /* Open the directory. */
    directory = opendir(input_dir);
    if(directory == NULL)
    {
        output(ERROR, "Can't open dir: %s\n", strerror(errno));
        return (-1);
    }
    
    /* Walk the directory. */
    while((entry = readdir(directory)) != NULL)
    {
    	/* Skip hidden files. */
        if(entry->d_name[0] == '.')
            continue;
        
        /* Copy file path. */
        rtrn = asprintf(&file_path, "%s/%s", input_dir, entry->d_name);
        if(rtrn < 0)
        {
            /* Ignore error for closedir() because we are going to quit anyway. */
            closedir(directory);
            return (-1);
        }

        /* Set index element to the file path we just copied. */
        file_index[index_counter] = file_path;

        /* Increment the index counter. */
        index_counter++;
    }
    
    /* Close the directory. */
    rtrn = closedir(directory);
    if(rtrn < 0)
    {
        output(ERROR, "Can't close directory\n");
        return (-1);
    }

	return (0);
}

static int32_t count_files_directory(uint32_t *count)
{
    int32_t rtrn = 0;
    DIR *directory = NULL;
	struct dirent *entry = NULL;

    /* Open the directory. */
    directory = opendir(input_dir);
    if(directory == NULL)
    {
        output(ERROR, "Can't open dir: %s\n", strerror(errno));
        return (-1);
    }
    
    /* Walk the directory. */
    while((entry = readdir(directory)) != NULL)
    {
    	/* Skip hidden files. */
        if(entry->d_name[0] == '.')
            continue;

        /* Increment the file count. */
        (*count)++;
    }
    
    /* Close the directory. */
    rtrn = closedir(directory);
    if(rtrn < 0)
    {
        output(ERROR, "Can't close directory\n");
        return (-1);
    }

	return (0);
}

int initial_fuzz_run(void)
{
    uint32_t i;

    for(i = 0; i < file_count; i++)
    {
        
    }

	return 0;
}

static void *kill_test_proc(void *kill_pid)
{
    sleep(1);

    pid_t *proc_pid = (pid_t *)kill_pid;

    int32_t rtrn = kill(*proc_pid, SIGKILL);
    if(rtrn < 0)
    {
        output(ERROR, "Can't kill child process: %s\n", strerror(errno));
        return NULL;
    }

    return NULL;
}

int32_t set_end_offset(uint64_t offset)
{
    return 0;
}

static int32_t test_exec_with_file_in_child(char *file_path, char *file_extension)
{
    int rtrn;
    pid_t child_pid;
    char *out_path auto_clean = NULL;
    char *file_name auto_clean = NULL;

    child_pid = fork();
    if(child_pid == 0)
    {
        if(atomic_load(&map->stop) == TRUE)
             _exit(0);

        char * const argv[] = {file_path, NULL};

        /* Execute the target executable with the file we generated. */
        rtrn = execv(path_to_exec, argv);
        if(rtrn < 0)
        {
            output(ERROR, "Can't execute target program: %s\n", strerror(errno));
            return -1;
        }

        _exit(-1);
    }
    else if(child_pid > 0)
    {
        int status;
        pthread_t kill_thread;
        
        /* Create a thread to kill the other process if it does not crash. */
        rtrn = pthread_create(&kill_thread, NULL, kill_test_proc, &child_pid);
        if(rtrn < 0)
        {
            output(ERROR, "Can't create kill thread\n");
            return -1;
        }

        /* Wait for test program to crash or be killed by the kill thread. */
        waitpid(child_pid, &status, 0);

        /* Check if the target program recieved a signal. */
        if(WIFSIGNALED(status) == TRUE)
        {
            /* Check the signal the target program recieved. */
            switch(WTERMSIG(status))
            {
                /* We caused the signal so ignore it. */
                case SIGKILL:
                    break;

                /* The program we are testing crashed let's save the file
                that caused the crash.  */
                case SIGSEGV:
                case SIGBUS:

                    /* Create a random file name. */
                    rtrn = generate_name(&file_name, file_extension, FILE_NAME);
                    if(rtrn < 0)
                    {
                        output(ERROR, "Can't create out path: %s\n", strerror(errno));
                        return -1;
                    }

                    /* Create a file path.  */
                    rtrn = asprintf(&out_path, "%s/crash_dir/%s", file_path, file_name);
                    if(rtrn < 0)
                    {
                        output(ERROR, "Can't create out path: %s\n", strerror(errno));
                        return -1;
                    }

                    /* Copy file out of temp into the out directory. */
                    rtrn = copy_file_to(file_path, out_path);
                    if(rtrn < 0)
                    {
                        output(ERROR, "Can't copy file to crash directory\n");
                        return -1;
                    }

                    break;
                
                /* We recieved a less interesting signal let's save
                 that in a different directory. */
                default:
                   break;

            }
        }
        int *response;

        pthread_join(kill_thread, (void **)&response);
            
    }
    else
    {
        output(ERROR, "Can't create child process: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

void start_main_file_loop(void)
{
    output(STD, "Starting fuzzer\n");

    /* Set up signal handler. */
    setup_signal_handler();

    /* Check if we should stop or continue running. */
    while(atomic_load(&map->stop) == FALSE)
    {
        /* Our variables. */
        int rtrn, file;
        char *file_buffer, *file_name, *file_path, *file_extension;
        uint64_t file_size;

        /* Open file from in directory. */
        rtrn = get_file(&file, &file_extension);
        if(rtrn < 0)
        {
            output(ERROR, "Can't get file\n");
            return;
        }

        /* Read file into memory. */
        rtrn = map_file_in(file, &file_buffer, &file_size);
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

        /* Check if smart mode is on. */
        if(map->smart_mode == TRUE)
        {
            /* Randomly join this file we mutated with a file from the old generation. */
            rtrn = create_new_generation(&file_buffer, &file_size, file_extension);
            if(rtrn < 0)
            {
                output(ERROR, "Can't create a new generation\n");
                return;
            }
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
        rtrn = test_exec_with_file_in_child(file_path, file_extension);
        if(rtrn < 0)
        {
            output(ERROR, "Can't test exec with file\n");
            return;
        }

        /* Clean up our mess. */
        mem_free(file_path);
        mem_free(file_name);
        mem_free(file_extension);
        mem_free_shared(file_buffer, (size_t)file_size);
        close(file);
    }

    output(STD, "Exiting main loop\n");

    return;
}

int32_t setup_file_module(char *exec_path, char *input)
{
    int32_t rtrn = 0;
    uint32_t i = 0;

    /* Set exec path. */
    path_to_exec = exec_path;

    /* Set input directory. */
    input_dir = input;

    /* Count how many files are in the input directory. */
    rtrn = count_files_directory(&file_count);
    if(rtrn < 0)
    {
        output(ERROR, "Can't count files in the in directory.\n");
        return (-1);
    }

    /* Allocate file array. */
    file_index = mem_alloc((file_count + 1) * sizeof(char *));
    if(file_index == NULL)
    {
        output(ERROR, "Can't create file index: %s\n", strerror(errno));
        return (-1);
    }

    /* Allocate the indivual array indicie. */
    for(i = 0; i < file_count; i++)
    {
        file_index[i] = mem_alloc(1025);
        if(file_index[i] == NULL)
        {
            output(ERROR, "Can't create shared object\n");
            return (-1);
        }
    }

    /* Create a index with all the file paths in the input directory. */
    rtrn = create_file_index();
    if(rtrn < 0)
    {
        output(ERROR, "Can't create file index\n");
        return (-1);
    }
    
    return (0);
}


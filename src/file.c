

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

#include "file.h"
#include "mutate.h"
#include "signals.h"
#include "genetic.h"
#include "crypto.h"
#include "nextgen.h"
#include "io.h"

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <dirent.h>

int get_file(int *file, char **extension)
{
	int rtrn;
	unsigned int index_offset;

    /* Pick a file index offset at random. */
	rtrn = rand_range(map->file_count, &index_offset);
	if(rtrn < 0)
	{
		output(ERROR, "Can't pick random number\n");
		return -1;
	}

    /* Open the file at the random offset. */
	*file = open(map->file_index[index_offset], O_RDONLY);
	if(*file < 0)
	{
		output(ERROR, "Can't open file: %s\n", strerror(errno));
		return -1;
	}

    /* Get the file extension for the file we just opened. */
	rtrn = get_extension(map->file_index[index_offset], extension);
	if(rtrn < 0)
	{
		output(ERROR, "Can't get extension\n");
		return -1;
	}
	
    return 0;
}

int create_file_index(void)
{
	struct dirent *entry;
	unsigned int index_counter = 0;
	char *file_path;
    DIR *directory;
    int rtrn;

    /* Open the directory. */
    directory = opendir(map->path_to_in_dir);
    if(directory == NULL)
    {
        output(ERROR, "Can't open dir: %s\n", strerror(errno));
        return -1;
    }
    
    /* Walk the directory. */
    while((entry = readdir(directory)) != NULL)
    {
    	/* Skip hidden files. */
        if(entry->d_name[0] == '.')
        {
            continue;
        }
        
        /* Copy file path. */
        rtrn = asprintf(&file_path, "%s/%s", map->path_to_in_dir, entry->d_name);
        if(rtrn < 0)
        {
            closedir(directory);
            return -1;
        }

        /* Set slot in index to the file path we just copied. */
        map->file_index[index_counter] = file_path;

        /* Increment the index counter. */
        index_counter++;
    }
    
    /* Close the directory. */
    closedir(directory);

	return 0;
}

int count_files_directory(unsigned int *count)
{
	struct dirent *entry;
    DIR *directory;

    /* Open the directory. */
    directory = opendir(map->path_to_in_dir);
    if(directory == NULL)
    {
        output(ERROR, "Can't open dir: %s\n", strerror(errno));
        return -1;
    }
    
    /* Walk the directory. */
    while((entry = readdir(directory)) != NULL)
    {
    	/* Skip hidden files. */
        if(entry->d_name[0] == '.')
        {
            continue;
        }

        /* Increment the file count. */
        *count = *count + 1;
    }
    
    /* Close the directory. */
    closedir(directory);

	return 0;
}

int initial_fuzz_run(void)
{
	unsigned int file_count = map->file_count;
    unsigned int i;

    for(i = 0; i < file_count; i++)
    {
        
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
        off_t file_size;

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
        free(file_path);
        free(file_name);
        free(file_extension);
        munmap(file_buffer, (size_t)file_size);
        close(file);
    }

    output(STD, "Exiting main loop\n");

    return;
}

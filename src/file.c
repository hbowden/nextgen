

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
#include "crypto.h"
#include "nextgen.h"
#include "io.h"

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
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

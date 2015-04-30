
#include "disas.h"
#include "distorm.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>

int disas_executable(char *path)
{
    int fd, rtrn;
    char *file_buffer;
    off_t size;

    fd = open(path, O_RDONLY);
    if(fd < 0)
    {
    	perror("open");
    	return -1;
    }

    rtrn = map_file_in(fd, &file_buffer, &size);
    if(rtrn < 0)
    {
    	printf("Can't memory map file\n");
    	return -1;
    }

    
    


    return 0;
}
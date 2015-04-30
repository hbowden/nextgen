

#include "utils.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

int get_file_size(int fd, off_t *size)
{
    struct stat stbuf;

    if((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
    {
        perror("fstat");
        return -1;
    }

    *size = stbuf.st_size;

    return 0;
}

int map_file_in(int *fd, char **buf, off_t *size)
{
	int rtrn;

    rtrn = get_file_size(*fd, size);
    if(rtrn < 0)
    {
    	printf("Can't get file size\n");
    	return -1;
    }

    *buf = mmap(0, (unsigned long) *size, PROT_READ | PROT_WRITE, MAP_PRIVATE, *fd, 0);
    if(*buf == MAP_FAILED)
    {
    	perror("mmap");
    	return -1;
    }

	return 0;
}

int map_file_out(char *path, char *buf, off_t *size)
{
    int fd;

    fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if(fd < 0)
    {
    	perror("open");
    	return -1;
    }

    if(lseek(fd, *size - 1, SEEK_SET) == -1)
    {
    	perror("lseek");
    	return -1;
    }

    ssize_t ret = write(fd, "", 1);
    if(ret != 1)
    {
    	perror("write");
    	return -1;
    }

    char *dst = mmap(0, (unsigned long) size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if(dst == MAP_FAILED)
    {
    	perror("mmap");
    	return -1;
    }

    memcpy(dst, buf, (unsigned long)size);

    munmap(dst, (unsigned long)size);

    close(fd);

	return 0;
}

void output(enum out_type type, const char *format, ...)
{
	va_list args;

	va_start(args, format);

	switch((int)type)
	{
		case ERROR:
		    vfprintf(stderr, format, args);
		    break;

		case STD:
		    vfprintf(stdout, format, args);
		    break;
	}

	va_end(args);

	return;
}

int get_core_count(unsigned int *core_count)
{
    int mib[4];

    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;

    size_t len = sizeof(unsigned int);

    int rtrn = sysctl(mib, 2, core_count, &len, NULL, 0);
    if(rtrn < 0)
    {
        perror("sysctl");
        return -1;
    }

    if(*core_count < 1)
    {
    	core_count = 1;
    }

	return;
}
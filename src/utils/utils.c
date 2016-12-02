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

#ifdef MAC_OSX

#include <sys/syslimits.h>

#else

/* We need to define _GNU_SOURCE to use
 asprintf on Linux. We also need to place
 _GNU_SOURCE at the top of the file before
 any other includes for it to work properly. */
#define _GNU_SOURCE

#endif

#include "utils.h"
#include "autoclose.h"
#include "autofree.h"
#include "crypto/crypto.h"
#include "io/io.h"
#include "memory/memory.h"

#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fts.h>
#include <err.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

// /* Compile in run_syscall() for macOS because syscall(2) is deprecated
//    since macOS sierra (10.12). */
// #if __x86_64__
//
// #include <stdarg.h>
//
// int32_t syscall(int32_t number, ...)
// {
//     (void)number;
//     __asm__ ( "movl $10, %eax;"
//               "movl $20, %ebx;"
//               "addl %ebx, %eax;"
//     );
//     return (0);
// }
//
// #endif

int32_t map_file_in(int32_t fd, char **buf, uint64_t *size, int32_t perm, struct output_writter *output)
{
    int32_t rtrn = 0;

    /* Get the file's size. */
    rtrn = get_file_size(fd, size, output);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't get file size\n");
        return (-1);
    }

    /* Use MAP_SHARED otherwise the file won't change when we write it to disk. */
    (*buf) = mmap(0, (unsigned long)(*size), perm, MAP_SHARED, fd, 0);
    if((*buf) == MAP_FAILED)
    {
        output->write(ERROR, "mmap: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

int32_t map_file_out(char *path, char *buf, uint64_t size, struct output_writter *output)
{
    int32_t fd auto_close = 0;

    fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if(fd < 0)
    {
        output->write(ERROR, "open: %s\n", strerror(errno));
        return (-1);
    }

    if(lseek(fd, (int64_t)size - 1, SEEK_SET) == -1)
    {
        output->write(ERROR, "lseek: %s\n", strerror(errno));
        return (-1);
    }

    ssize_t ret = write(fd, "", 1);
    if(ret != 1)
    {
        output->write(ERROR, "write: %s\n", strerror(errno));
        return (-1);
    }

    char *dst = mmap(0, (unsigned long)size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(dst == MAP_FAILED)
    {
        output->write(ERROR, "mmap: %s\n", strerror(errno));
        return (-1);
    }

    memcpy(dst, buf, (unsigned long)size);
    munmap(dst, (unsigned long)size);

    return (0);
}

int32_t get_file_size(int32_t fd, uint64_t *size, struct output_writter *output)
{
    struct stat stbuf;

    if((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
    {
        output->write(ERROR, "fstat: %s\n", strerror(errno));
        return (-1);
    }

    (*size) = (uint64_t)stbuf.st_size;

    return (0);
}

int32_t copy_file_to(char *src, char *dst, struct output_writter *output)
{
    int32_t rtrn = 0;
    uint64_t file_size = 0;
    char *file_buffer = NULL;
    int32_t file auto_close = 0;

    file = open(src, O_RDONLY);
    if(file < 0)
    {
        output->write(ERROR, "Can't open file: %s\n", strerror(errno));
        free(file_buffer);
        return (-1);
    }

    rtrn = map_file_in(file, &file_buffer, &file_size, READ, output);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't read file to memory\n");
        return (-1);
    }

    rtrn = map_file_out(dst, file_buffer, file_size, output);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't write file to disk\n");
        munmap(file_buffer, (size_t)file_size);
        return (-1);
    }

    rtrn = fsync(file);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't sync file on disk\n");
        munmap(file_buffer, (size_t)file_size);
        return (-1);
    }

    munmap(file_buffer, (size_t)file_size);

    return (0);
}

int32_t check_root(void)
{
    printf("Making sure nextgen has root privileges\n");

    uid_t check = 0;

    check = getuid();
    if(check == 0)
        return (0);

    return (-1);
}

/* Drop root privileges. */
int32_t drop_privileges(void)
{
    int32_t rtrn = 0;

    rtrn = setgid(999);
    if(rtrn < 0)
    {
        printf("Can't drop gid: %s\n", strerror(errno));
        return (-1);
    }

    rtrn = setuid(999);
    if(rtrn < 0)
    {
        printf("Can't drop uid: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

int32_t get_extension(char *path, char **extension)
{
    char *pointer = strrchr(path, '.');
    if(pointer == NULL)
    {
        printf("Can't find . %s:\n", strerror(errno));
        return (-1);
    }

    int32_t rtrn = asprintf(extension, "%s\n", pointer + 1);
    if(rtrn < 0)
    {
        printf("Can't alloc extension: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

int32_t generate_file_name(char **name, char *extension, struct output_writter *output, struct random_generator *random)
{
    (void)name;
    (void)extension;
    (void)output;
    (void)random;
    return (0);
}

int32_t generate_directory_name(char **name, struct output_writter *output)
{
    (void)name;
    (void)output;
    return (0);
}

int32_t get_home(char **home)
{
    uid_t uid = 0;
    struct passwd *pwd = NULL;
    int32_t rtrn = 0;

    /* Get User UID */
    uid = getuid();

    /* Get user password struct */
    if(!(pwd = getpwuid(uid)))
    {
        printf("Can't Get pwd struct: %s\n", strerror(errno));
        return (-1);
    }

    /* Copy User Home Directory to buffer supplied to function */
    rtrn = asprintf(home, "%s", pwd->pw_dir);
    if(rtrn < 0)
    {
        printf("Can't Create home string: %s\n", strerror(errno));
        return (-1);
    }

    /* Exit function. */
    return (0);
}

int32_t ascii_to_binary(char *input, char **out, uint64_t input_len,
                        uint64_t *out_len)
{
    uint32_t i;

    /* Make sure length is not zero. */
    if(input_len == 0)
    {
        printf("Length argument is zero\n");
        return (-1);
    }

    /* Calculate the output string length.
     Should probably be switched to equivalent division. */
    (*out_len) = input_len * 8;

    /* Allocate the output buffer. */
    (*out) = malloc((*out_len) + 1);
    if((*out) == NULL)
    {
        printf("Can't allocate binary string\n");
        return (-1);
    }

    /* Loop and convert to a binary string. */
    for(i = 0; i < input_len; i++)
    {
        char ch = input[i];
        char *o = &(*out)[8 * i];
        unsigned char b;

        for(b = 0x80; b; b >>= 1)
            *o++ = ch & b ? '1' : '0';
    }

    /* null terminate the binary string. */
    (*out)[(*out_len)] = '\0';

    /* Return zero. */
    return (0);
}

int32_t create_random_file(char *root,
                           char *ext,
                           char **path,
                           uint64_t *size,
                           struct random_generator *random,
                           struct memory_allocator *allocator,
                           struct output_writter *output)
{
    int32_t rtrn = 0;
    int32_t no_period = 0;
    char *name auto_free = NULL;
    char *junk auto_free = NULL;
    char *extension auto_free = NULL;

    /* Check for a period in the extension string passed by the user. */
    char *pointer = strrchr(ext, '.');
    if(pointer == NULL)
    {
        /* There's no period so let's create one. */
        rtrn = asprintf(&extension, ".%s", ext);
        if(rtrn < 0)
        {
            output->write(ERROR, "Can't create extension string\n");
            return (-1);
        }

        no_period = 1;
    }

    if(no_period == 1)
    {
        /* Generate random file name. */
        rtrn = generate_file_name(&name, extension, output, random);
        if(rtrn < 0)
        {
            output->write(ERROR, "Can't generate random file name\n");
            return (-1);
        }
    }
    else
    {
        /* Generate random file name. */
        rtrn = generate_file_name(&name, ext, output, random);
        if(rtrn < 0)
        {
            output->write(ERROR, "Can't generate random file name\n");
            return (-1);
        }
    }

    /* Join paths together. */
    rtrn = asprintf(path, "%s/%s", root, name);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't join paths: %s\n", strerror(errno));
        return (-1);
    }

    /* Pick a random size between zero and 4 kilobytes. */
    rtrn = random->range(4095, (uint32_t *)size);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't choose random number\n");
        return (-1);
    }

    /* Add one to size so it's not zero. */
    (*size) = (*size) + 1;

    /* Allocate a buffer to put junk in. */
    junk = allocator->alloc((*size) + 1);
    if(junk == NULL)
    {
        output->write(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
        return (-1);
    }

    /* Put some junk in a buffer. */
    rtrn = random->bytes(allocator, output, &junk, (uint32_t)(*size));
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't alloc junk buf: %s\n", strerror(errno));
        return (-1);
    }

    /* Write that junk to the file so that the file is not just blank. */
    rtrn = map_file_out((*path), junk, (*size), output);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't write junk to disk\n");
        return (-1);
    }

    return (0);
}

int32_t binary_to_ascii(char *input, char **out, uint64_t input_len,
                        uint64_t *out_len)
{
    (void)input;

    if(input_len == 0)
    {
        printf("Input length is zero\n");
        return (-1);
    }

    (*out_len) = input_len / 8;

    (*out) = malloc((*out_len) + 1);
    if((*out) == NULL)
    {
        printf("Can't allocate binary string\n");
        return (-1);
    }

    (*out)[(*out_len)] = '\0';

    return (0);
}

#ifdef FREEBSD

static int32_t entcmp(const FTSENT *const *a, const FTSENT *const *b)
{
    return (strcmp((*a)->fts_name, (*b)->fts_name));
}

#else

static int32_t entcmp(const FTSENT **a, const FTSENT **b)
{
    return strcmp((*a)->fts_name, (*b)->fts_name);
}

#endif

int32_t delete_dir_contents(char *dir)
{
    FTS *tree;
    FTSENT *f;
    int32_t rtrn = 0;
    char *argv[] = { dir, NULL };

    tree = fts_open(argv, FTS_LOGICAL | FTS_NOSTAT, entcmp);
    if(tree == NULL)
    {
        printf("Can't walk directory\n");
        return (-1);
    }

    while((f = fts_read(tree)))
    {
        switch(f->fts_info)
        {
            case FTS_DNR:   /* Cannot read directory */
            case FTS_ERR:   /* Miscellaneous error */
            case FTS_NS:    /* stat() error */
                continue;
        }

        if(strcmp(f->fts_path, dir) == 0)
            continue;

        struct stat sb;

        rtrn = stat(f->fts_path, &sb);
        if(rtrn < 0)
        {
            printf("Can't get path stats: %s\n", strerror(errno));
            return (-1);
        }

        /* Check if we have a directory. */
        if(sb.st_mode & S_IFDIR)
        {
            rtrn = rmdir(f->fts_path);
            if(rtrn < 0)
                continue;
        }
        else
        {
            rtrn = unlink(f->fts_path);
            if(rtrn < 0)
            {
                printf("Can't remove file\n");
                return (-1);
            }
        }
    }

    rtrn = fts_close(tree);
    if(rtrn < 0)
    {
        printf("fts: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

int32_t delete_directory(char *path)
{
    /* Check for a NULL pointer being passed to us. */
    if(path == NULL)
    {
        printf("Path pointer is NULL\n");
        return (-1);
    }

    struct stat sb;
    int32_t rtrn = 0;

    /* Get filesystem stats for the path supplied. */
    rtrn = stat(path, &sb);
    if(rtrn < 0)
    {
        printf("Can't get stats: %s\n", strerror(errno));
        return(-1);
    }

    /* Make sure path is a directory. */
    if(sb.st_mode & S_IFDIR)
    {
        /* Delete the contents of the directory before we
        try deleting the directory it's self. */
        rtrn = delete_dir_contents(path);
        if(rtrn < 0)
        {
            printf("Can't delete dir contents\n");
            return (-1);
        }
    }
    else
    {
        printf("Input path is not a directory\n");
        return (-1);
    }

    rtrn = rmdir(path);
    if(rtrn < 0)
    {
        printf("Can't remove directory: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

int32_t count_files_directory(uint32_t *count, char *dir)
{
    struct stat buf;
    int32_t rtrn = 0;
    DIR *directory = NULL;
    char *file_path auto_free = NULL;
    struct dirent *entry = NULL;

    /* Open the directory. */
    directory = opendir(dir);
    if(directory == NULL)
    {
        printf("Can't open dir: %s\n", strerror(errno));
        return (-1);
    }

    /* Walk the directory. */
    while((entry = readdir(directory)) != NULL)
    {
        /* Skip hidden files. */
        if(entry->d_name[0] == '.')
            continue;

        /* Create file path. */
        rtrn = asprintf(&file_path, "%s/%s", dir, entry->d_name);
        if(rtrn < 0)
        {
            /* Ignore error for closedir() because we are going to quit anyway. */
            closedir(directory);
            return (-1);
        }

        /* Grab file stats. */
        rtrn = stat(file_path, &buf);
        if(rtrn < 0)
        {
            printf("Can't get file stats: %s\n", strerror(errno));
            closedir(directory);
            return (-1);
        }

        /* Make sure the path is a file if not skip and continue. */
        if(buf.st_mode & S_IFREG)
        {
            /* Increment the file count. */
            (*count)++;
        }
    }

    /* Close the directory. */
    rtrn = closedir(directory);
    if(rtrn < 0)
    {
        printf("Can't close directory\n");
        return (-1);
    }

    return (0);
}

int32_t create_random_directory(char *root, char **path, struct output_writter *output)
{
    int32_t rtrn = 0;
    char *name = NULL;

    rtrn = generate_directory_name(&name, output);
    if(rtrn < 0)
    {
        printf("Failed to generate directory name\n");
        return (-1);
    }

    rtrn = asprintf(path, "%s/%s", root, name);
    if(rtrn < 0)
    {
        printf("String concatenation failed\n");
        return (-1);
    }

    rtrn = mkdir((*path), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(rtrn < 0)
    {
        printf("Directory creation failed: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

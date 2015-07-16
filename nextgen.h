

#ifndef NEXTGEN_H
#define NEXTGEN_H

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define FALSE 0
#define TRUE 1

#define MODE_SYSCALL 0
#define MODE_NETWORK 1
#define MODE_FILE 2

#define autofree __attribute((cleanup(__autofree)))

static inline void __autofree(void *pointer)  
{
    free(pointer);
    return;
}

struct executable_context
{
    pid_t pid;

    /* The virtual memory address found in the execuatble header. */
    unsigned long main_start_address;

    /* The virtual memory offset of where the executable begins. */
    unsigned long start_offset;

    unsigned long end_offset;

    unsigned long number_of_branchs;

    char *args[];
};

struct child_proc
{
    pid_t child_pid;
};

struct shared_map
{
    int mode;
    char *path_to_in_dir;
    char *path_to_out_dir;
    char *path_to_exec;

    struct executable_context *exec_ctx;

    pid_t reaper_pid;
    pid_t runloop_pid;

    char *crypto_method;
    bool crypto_flag;

    bool stop;

    unsigned int number_of_children;
    struct child_proc **children[];
};

extern struct shared_map *map;

#endif
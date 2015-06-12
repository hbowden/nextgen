

#ifndef NEXTGEN_H
#define NEXTGEN_H

#include <unistd.h>
//#include <stdatomics.h>
#include <stdbool.h>

#define FALSE 0
#define TRUE 1

struct executable_context
{
    unsigned char *main_start_address;

    unsigned long number_of_branchs;

};

struct child_proc
{
    pid_t child_pid;
};

struct shared_map
{
    char *path_to_dir;
    char *path_to_exec;
    char *bin_start_addr;
    char *bin_end_addr;

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
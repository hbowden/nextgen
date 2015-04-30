
#ifndef TYPES_H
#define TYPES_H

struct shared_map
{
    char *path_to_dir;
    char *path_to_exec;
    char *bin_start_addr;
    char *bin_end_addr;
};

#endif
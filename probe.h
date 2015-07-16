

#ifndef PROBE_H
#define PROBE_H

#include "private.h"

#define REGISTER_IP RIP
#define TRAP_LEN    1
#define TRAP_INST   0xCC
#define TRAP_MASK   0xFFFFFFFFFFFFFF00

private extern int inject_probes(void);

private extern int inject_fork_server(void);

#endif

#ifndef NX_JOB_CTX_H
#define NX_JOB_CTX_H

enum job_type { GENESIS, NEW_GENERATION };

struct job_ctx
{
    uint64_t **args;

    uint32_t number_of_args;

    uint32_t syscall_number;

    enum job_type type;

    const char padding[4];
};

#endif

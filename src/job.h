
#ifndef NX_JOB_CTX_H
#define NX_JOB_CTX_H

enum job_type { GENESIS, NEW_GENERATION };

struct job_ctx
{
	enum job_type type;

    uint64_t **args;

    uint32_t number_of_args;

    uint32_t syscall_number;
};

#endif

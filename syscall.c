

#include "syscall.h"
#include "entry.h"
#include "syscall_table.h"
#include "crypto.h"
#include "utils.h"
#include "shim.h"

void get_syscall_table(void)
{
	/* Set the syscall table with the operating specific get_table() function. */
    map->sys_table = get_table();

	return;
}

int pick_syscall(struct child_ctx *ctx)
{
    int rtrn;

    unsigned int syscall_number;

    rtrn = rand_range(ctx->sys_table->number_of_syscalls, &syscall_number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }

    ctx->syscall_number = syscall_number + 1;

    return 0;
}

int generate_arguments(struct child_ctx *ctx)
{
    unsigned int i;
    unsigned int number_of_args = ctx->sys_table->sys_entry[ctx->syscall_number].number_of_args;
    int rtrn;

    for(i = 0; i < number_of_args; i++)
    {
    	/* This crazy line allows us to avoid a large switch stament in the code. */
        rtrn = ctx->sys_table->sys_entry[ctx->syscall_number].get_arg_index[i](&ctx->arg_value_index[i]);
        if(rtrn < 0)
        {
        	output(ERROR, "Can't generate arguments for: %s\n", ctx->sys_table->sys_entry[ctx->syscall_number].name_of_syscall);
            return -1;
        }
    }

	return 0;
}


#include "syscall.h"
#include "entry.h"
#include "syscall_table.h"
#include "crypto.h"
#include "utils.h"
#include "shim.h"

#include <string.h>

int get_syscall_table(void)
{
    output(STD, "Building system call table\n");

	/* Grab a copy of the syscall table in on disk format. */ 
	struct syscall_table *sys_table = get_table();
    if(sys_table == NULL)
    {
        output(STD, "Can't grab syscall table\n");
        return -1;
    }

    /* Create a shadow syscall table.*/
    struct syscall_table_shadow *shadow_table = malloc(sizeof(struct syscall_table_shadow));
    if(shadow_table == NULL)
    {
        output(ERROR, "Can't create shadow table\n");
        return -1;
    }

    shadow_table->number_of_syscalls = sys_table->number_of_syscalls;

    unsigned int i, ii;

    /* Allocate heap memory for the list of syscalls. */
    shadow_table->sys_entry = malloc(shadow_table->number_of_syscalls * sizeof(struct syscall_entry_shadow));
    if(shadow_table->sys_entry == NULL)
    {
        output(ERROR, "Can't create new entry\n");
        return -1;
    }

    /* Loop for each entry syscall and build a table from the on disk format. */
    for(i = 0; i < shadow_table->number_of_syscalls; i++)
    {
        struct syscall_entry_shadow entry = {  .number_of_args = sys_table[i + 1].sys_entry->number_of_args, .name_of_syscall = sys_table[i + 1].sys_entry->name_of_syscall };

        for(ii = 0; ii < entry.number_of_args; ii++)
        {
            output(STD, "table: %p\n", sys_table[ii + 1]);
            output(STD, "entry: %p\n", sys_table[ii + 1].sys_entry);
            output(STD, "index: %p\n", sys_table[ii + 1].sys_entry->get_arg_index[ii]);

            entry.get_arg_index[ii] = sys_table[i + 1].sys_entry->get_arg_index[ii];
        }
        
        if(sys_table[i + 1].sys_entry->status == ON)
        {
            atomic_init(&entry.status, ON);
        }
        else
        {
            atomic_init(&entry.status, OFF);	
        }

        shadow_table->sys_entry[i] = entry;
    }

    map->sys_table = shadow_table;

	return 0;
}

/* This function is used to randomly pick the syscall to test. */
int pick_syscall(struct child_ctx *ctx)
{
	/* Our variables we will be using. */
    int rtrn;

    /* Use rand_range to pick a number between 0 and the number_of_syscalls.  */
    rtrn = rand_range(map->sys_table->number_of_syscalls, &ctx->syscall_number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }

    return 0;
}

int generate_arguments(struct child_ctx *ctx)
{
    unsigned int i;

    unsigned int number_of_args = map->sys_table->sys_entry[ctx->syscall_number].number_of_args;
    int rtrn;

    for(i = 0; i < number_of_args; i++)
    {
    	/* This crazy line allows us to avoid a large switch stament in the code. */
        rtrn = map->sys_table->sys_entry[ctx->syscall_number].get_arg_index[i](&ctx->arg_value_index[i]);
        if(rtrn < 0)
        {
        	output(ERROR, "Can't generate arguments for: %s\n", map->sys_table->sys_entry[ctx->syscall_number].name_of_syscall);
            return -1;
        }
    }

	return 0;
}
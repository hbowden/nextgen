

#include "syscall.h"
#include "syscall_table.h"
#include "shim.h"

void get_syscall_table(void)
{
	/* Set the syscall table with the operating specific get_table() function. */
    map->sys_table = get_table();

	return;
}
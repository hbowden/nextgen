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

#include "syscall.h"
#include "child.h"
#include "arg_types.h"
#include "crypto/crypto.h"
#include "entry.h"
#include "io/io.h"
#include "genetic/job.h"
#include "log/log.h"
#include "memory/memory.h"
#include "mutate/mutate.h"
#include "runtime/nextgen.h"
#include "runtime/platform.h"
#include "probe/probe.h"
#include "resource/resource.h"
#include "signals.h"
#include "set_test.h"
#include "syscall_table.h"
#include "utils/utils.h"
#include "utils/noreturn.h"
#include "concurrent/concurrent.h"

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <setjmp.h>
#include <signal.h>

struct child_ctx
{
    /* The child's process PID. */
    int32_t pid;

    /* A varible to store the address of where to jump back to in the child
    process on signals. */
    jmp_buf return_jump;

    /* This is the number used to identify and choose the syscall that's going to be tested. */
    uint32_t syscall_number;

    /* Symbolized syscall number. */
    int32_t syscall_symbol;

    /* The argument were currently generating. */
    uint32_t current_arg;

    /* The number of args for the syscall were testing. */
    uint32_t total_args;

    /* The name of the syscall were testing. */
    const char *syscall_name;

    /* This array is where we store the arguments we generate. */
    uint64_t **arg_value_array;

    /* This array is a copy of the value array. */
    uint64_t **arg_copy_array;

    /* This index tracks the size of the arguments.*/
    uint64_t *arg_size_array;

    /* Time that we made the syscall fuzz test. */
    struct timeval time_of_syscall;

    /* The return value of the last syscall called. */
    int32_t ret_value;

    /* If this is set we know the syscall failed. */
    int32_t had_error;

    int32_t status;

    int32_t sig_num;

    int32_t did_jump;

    const char padding1[4];

    struct probe_ctx *probe_handle;

    int32_t need_alarm;

    const char padding2[7];

    int32_t (*test_syscall)(int32_t, uint64_t **);
};

struct global_state
{
    /* The number of children processes currently running. */
    uint32_t running_children;

    /* Counter for number of syscall test that have been completed. */
    uint32_t *test_counter;
};

static struct global_state *state;

/* The total number of children process to run. */
static uint32_t total_children;

/* An array of child context structures. These structures track variables
   local to the child process. */
static struct child_ctx **children;

/* The syscall table. */
static struct syscall_table *sys_table;

/* A copy of the stop pointer. If the atomic int is
   set to NX_YES than we stop fuzzing and exit. */
static int32_t *stop;

/* This variable tells us wether were in smart mode or dumb mode. */
static int32_t mode;

static int8_t table_set;

static epoch_ctx *epoch;

int32_t get_arg_size(struct child_ctx *child, uint32_t arg_num, uint64_t *size, struct output_writter *output)
{
    /* Make sure the argument number requested is in bounds. */
    if(arg_num > ARG_LIMIT)
    {
        output->write(ERROR, "Arg number is greater than the arg limit.\n");
        return (-1);
    }

    (*size) = child->arg_size_array[arg_num];

    return (0);
}

uint32_t get_current_arg(struct child_ctx *child)
{
    return (child->current_arg);
}

void set_arg_size(struct child_ctx *child, uint64_t size)
{
    (void)child;
    (void)size;

    return;
}

void cleanup_syscall_table(struct syscall_table **table, struct memory_allocator *allocator)
{
    allocator->free_shared((void **)&(*table)->sys_entry, sizeof(struct syscall_entry));
    allocator->free_shared((void **)table, sizeof(struct syscall_table));

    return;
}

void get_total_syscalls(uint32_t *total)
{
    (*total) = atomic_load_uint32(&sys_table->total_syscalls);
    return;
}

struct syscall_entry *get_entry(uint32_t syscall_number)
{
    /* If the syscall number passed is greater than the total number
    of syscalls return NULL. */
    if(syscall_number > sys_table->total_syscalls)
        return (NULL);

   return(atomic_load_ptr(&sys_table->sys_entry[syscall_number]));
}

/* Don't use inside of the syscall module. This function is for
Other modules to get child processes. */
struct child_ctx *get_child_from_index(uint32_t i)
{
    /* Make sure the array offset is in bounds. */
    if(i > total_children)
        return (NULL);

    /* Return the child's context object pointer. */
    return (atomic_load_ptr(&children[i]));
}

static int32_t get_child_index_number(uint32_t *index_num)
{
    uint32_t i = 0;
    pid_t pid = getpid();

    /* Walk child array until our PID matches the one in the context struct. */
    for(i = 0; i < total_children; i++)
    {
        if(atomic_load_int32(&children[i]->pid) == pid)
        {
            /* The PIDS match so set the index number and exit the function. */
            (*index_num) = i;
            return (0);
        }
    }
    /* Should not get here, but if we do return an error. */
    return (-1);
}

NX_NO_RETURN static void exit_child(struct thread_ctx *thread, struct memory_allocator *allocator, struct output_writter *output)
{
    int32_t rtrn = 0;
    int32_t ret_val = 0;
    struct child_ctx *child = NULL;

    /* Start epoch protected section. */
    epoch_start(thread, allocator, output);

    /* Get our childs context object. */
    child = get_child(output);
    if(child == NULL)
    {
        output->write(ERROR, "Can't get child context\n");
        ret_val = -1;
        goto exit;
    }

    /* Clean up kernel probes. */
    rtrn = cleanup_kernel_probes(child->probe_handle);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't clean up kernel probes");
        ret_val = -1;
        goto exit;
    }

exit:
    /* We may be in a nested epoch section, so clean all current
    epoch sections before exiting. */
    stop_all_sections(thread, allocator);

    /* Clean the thread context object. */
    clean_thread(&thread, allocator);

    /* Set the PID as empty. */
    cas_loop_int32(&child->pid, EMPTY);

    /* Decrement the running child counter. */
    atomic_dec_uint32(&state->running_children);

    output->write(STD, "Exiting\n");

    /* Exit and cleanup child. */
    _exit(ret_val);
}

static int32_t free_old_arguments(struct child_ctx *ctx,
                                  struct output_writter *output,
                                  struct memory_allocator *allocator,
                                  struct resource_generator *rsrc_gen)
{
    uint32_t i;
    int32_t rtrn = 0;
    uint32_t total_args = ctx->total_args;
    struct syscall_entry *entry = NULL;

    /* Grab the syscall entry. */
    entry = get_entry(ctx->syscall_number);
    if(entry == NULL)
    {
        output->write(ERROR, "Can't get entry\n");
        return (-1);
    }

    for(i = 0; i < total_args; i++)
    {
        /* Handle args that require special cleanup procedures. */
        switch((int32_t)entry->arg_context_array[i]->type)
        {
            /* Below is the resource types ie they are from the resource module.
            They must be freed using special functions and the free must be done on
            the arg_copy_index so the free_* functions don't use the mutated value in arg_value_index. */
            case FILE_DESC:
                rtrn = rsrc_gen->free_desc((int32_t *)ctx->arg_copy_array[i]);
                if(rtrn < 0)
                    output->write(ERROR, "Can't free descriptor\n");
                /* Don't return on errors, just keep looping. */
                break;

            case FILE_PATH:
                rtrn = rsrc_gen->free_filepath((char **)&(ctx->arg_copy_array[i]));
                if(rtrn < 0)
                    output->write(ERROR, "Can't free filepath\n");
                /* Don't return on errors, just keep looping. */
                break;

            case DIR_PATH:
                rtrn = rsrc_gen->free_dirpath((char **)&(ctx->arg_copy_array[i]));
                if(rtrn < 0)
                    output->write(ERROR, "Can't free dirpath\n");
                /* Don't return on errors, just keep looping. */
                break;

            case SOCKET:
                rtrn = rsrc_gen->free_socket((int32_t *)ctx->arg_copy_array[i]);
                if(rtrn < 0)
                    output->write(ERROR, "Can't free socket\n");
                /* Don't return on errors, just keep looping. */
                break;
            /* End of resource types. */

            /* Clean this with mem_free_shared(). */
            case VOID_BUF:
                allocator->free_shared((void **)&ctx->arg_value_array[i], ctx->arg_size_array[i]);
                break;

            /* Kill the temp process using the copy value
               so that we don't use the mutated value
               in arg_value_index[i]. */
            case PID:
                /*rtrn = kill((pid_t)(*ctx->arg_copy_array[i]), SIGKILL);
                if(rtrn < 0)
                    output(ERROR, "Can't kill child: %s\n", strerror(errno));
                 Don't return on errors, just keep looping. */
                break;

            default:
                /* Free value if non NULL. */
                //mem_free((void **)&ctx->arg_value_array[i]);
                break;
        }
    }

    return (0);
}

NX_NO_RETURN static void start_smart_syscall_child(struct thread_ctx *thread,
                                                   struct memory_allocator *allocator,
                                                   struct output_writter *output)
{
    int32_t rtrn = 0;
    struct child_ctx *child = NULL;

    epoch_start(thread, allocator, output);

    /* Grab our child context object. */
    child = get_child(output);
    if(child == NULL)
    {
        output->write(ERROR, "Can't get child context\n");
        exit_child(thread, allocator, output);
    }

    /* Set the return jump so that we can try fuzzing again on a signal. This
    is required on some operating systems because they can't clean up old
    processes fast enough for us. It also alows us to do PRNG seeding and
    probe injection and teardown less often. */
    rtrn = setjmp(child->return_jump);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't set return jump\n");
        exit_child(thread, allocator, output);
    }

    epoch_stop(thread, allocator);

    /* Loop until ctrl-c is pressed by the user. */
    while(atomic_load_int32(stop) != TRUE)
    {

    }

    exit_child(thread, allocator, output);
}

struct child_ctx *get_child_ctx_from_pid(pid_t pid)
{
    uint32_t i;

    for(i = 0; i < total_children; i++)
    {
        if(children[i]->pid == pid)
            return (children[i]);
    }

    /* Should not get here, but if we do return NULL. */
    return (NULL);
}

struct child_ctx *get_child(struct output_writter *output)
{
    int32_t rtrn = 0;
    uint32_t offset = 0;

    /* Lookup our child's context object's offset in the child array. */
    rtrn = get_child_index_number(&offset);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't grab child number\n");
        return (NULL);
    }

    /* Load and return the actual child context pointer. */
    return (atomic_load_ptr(&children[offset]));
}

static int32_t generate_test_case(struct child_ctx *child,
                                  struct thread_ctx *thread,
                                  struct output_writter *output,
                                  struct memory_allocator *allocator,
                                  struct random_generator *random)
{
    int32_t rtrn = 0;
    struct syscall_entry *entry = NULL;

    /* Randomly pick the syscall to test. */
    rtrn = pick_syscall(child, random, output);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't pick syscall to test\n");
        exit_child(thread, allocator, output);
    }

    /* Generate arguments for the syscall selected. */
    rtrn = generate_arguments(child, output);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't generate arguments\n");
        exit_child(thread, allocator, output);
    }

    /* Mutate the arguments randomly. */
    rtrn = mutate_arguments(child->arg_value_array, child->arg_size_array, random, output);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't mutate arguments\n");
        exit_child(thread, allocator, output);
    }

    /* Grab the syscall entry for the syscall we picked. */
    entry = get_entry(child->syscall_number);
    if(entry == NULL)
    {
        output->write(ERROR, "Can't get syscall entry\n");
        exit_child(thread, allocator, output);
    }

    /* Log the arguments before we use them, in case we cause a
       kernel panic, so we know what caused the panic. */
    rtrn = log_arguments(child->total_args, child->syscall_name,
                         child->arg_value_array, entry->arg_context_array, allocator);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't log arguments\n");
        exit_child(thread, allocator, output);
    }

    return (0);
}

static struct syscall_table *build_syscall_table(struct output_writter *output, struct memory_allocator *allocator)
{
    struct syscall_table *table = NULL;
    struct syscall_table *syscall_table = NULL;

    /* Grab a copy of the syscall table in on disk format. */
    syscall_table = get_table();
    if(syscall_table == NULL)
    {
        output->write(ERROR, "Can't grab syscall table\n");
        return (NULL);
    }

    /* Create a syscall table object with shared memory.*/
    table = allocator->shared(sizeof(struct syscall_table));
    if(table == NULL)
    {
        output->write(ERROR, "Can't create shadow table\n");
        return (NULL);
    }

    /* Move the system call table into shared memory. */
    memmove(table, syscall_table, sizeof(struct syscall_table));

    uint32_t i, counter;

    for(i = 0; i < table->total_syscalls; i++)
    {
        struct syscall_entry *entry = table->sys_entry[i];
        uint32_t total_args = entry->total_args;
        set_test_syscall(entry, entry->id);
        entry->status = ON;

        for(counter = 0; counter < total_args; counter++)
        {
            int32_t type = entry->arg_type_array[counter];

            entry->arg_context_array[counter] = get_arg_context((enum arg_type)type, output);
            if(entry->arg_context_array[counter] == NULL)
            {
                output->write(ERROR, "Can't get argument context\n");
                return (NULL);
            }
        }
    }

    return (table);
}

struct syscall_table *get_syscall_table(struct output_writter *output, struct memory_allocator *allocator)
{
    if(table_set == TRUE)
    {
        /* If the sys_table pointer has already been set just
          return that, so we don't have different versions of
          the syscall table floating around. */
        return (atomic_load_ptr(&sys_table));
    }

    /* Build the syscall table and return it. */
    return (build_syscall_table(output, allocator));
}

/* This function is used to randomly pick the syscall to test. */
int32_t pick_syscall(struct child_ctx *child, struct random_generator *random, struct output_writter *output)
{
    /* Use rand_range to pick a number between 0 and the number_of_syscalls. The minus one
    is a hack, get_syscall_table() returns an array - 1 the size of number of syscalls
    and should be fixed. */

    uint32_t num = 0;

    int32_t rtrn = random->range(sys_table->total_syscalls - 1, &num);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't generate random number\n");
        return (-1);
    }

    /* Set syscall value's. */
    child->syscall_name = sys_table->sys_entry[num]->syscall_name;
    atomic_store_uint32(&child->syscall_number, num);
    atomic_store_uint32(&child->total_args, sys_table->sys_entry[num]->total_args);
    atomic_store_int32(&child->syscall_symbol, sys_table->sys_entry[num]->syscall_symbol);
    atomic_store_int32(&child->need_alarm, sys_table->sys_entry[num]->need_alarm);
    atomic_store_int32(&child->had_error, NX_NO);

    return (0);
}

int32_t generate_arguments(struct child_ctx *ctx, struct output_writter *output)
{
    uint32_t i = 0;
    int32_t rtrn = 0;
    uint32_t total_args = ctx->total_args;

    /* Loop for each syscall argument. */
    for(i = 0; i < total_args; i++)
    {
        /* Set the current argument number. */
        ctx->current_arg = i;

        struct syscall_entry *entry = get_entry(ctx->syscall_number);
        if(entry == NULL)
        {
            output->write(ERROR, "Can't get entry\n");
            return (-1);
        }

        /* Generate the argument. */
        rtrn = entry->get_arg_array[i](&ctx->arg_value_array[i], ctx);
        if(rtrn < 0)
        {
            output->write(ERROR, "Can't generate arguments for: %s\n", entry->syscall_name);
            return (-1);
        }

        /* Copy the argument into the argument copy array. */
        memcpy(ctx->arg_copy_array[i],
               ctx->arg_value_array[i],
               ctx->arg_size_array[i]);
    }

    return (0);
}

static int32_t check_for_failure(int32_t ret_value)
{
    if(ret_value < 0)
        return (-1);

    return (0);
}

int32_t test_syscall(struct child_ctx *ctx, struct output_writter *output)
{
    /* Check if we need to set the alarm for blocking syscalls.  */
    if(ctx->need_alarm == NX_YES)
        alarm(1);

    /* Get a copy of the syscall entry for the syscall we are testing.
       We need this to lookup the test_syscall() function. */
    struct syscall_entry *entry = get_entry(ctx->syscall_number);
    if(entry == NULL)
    {
        output->write(ERROR, "Can't get syscall entry\n");
        return (-1);
    }

    /* Do the add before the syscall test because we usually crash on the syscall test
    and we don't wan't to do this in a signal handler. */
    atomic_add_uint32(state->test_counter, 1);

    /* Set the time of the syscall test. */
    (void)gettimeofday(&ctx->time_of_syscall, NULL);

    /* Keep a reference of test syscall so we can cleanup entry before
     running test_syscall(). test_syscall() may crash so this allows us
     to free the syscall entry before and avoid a memory leak. */
    ctx->test_syscall = entry->test_syscall;

    /* Call the syscall with the args generated. */
    ctx->ret_value = ctx->test_syscall(ctx->syscall_symbol, ctx->arg_value_array);
    if(check_for_failure(ctx->ret_value) < 0)
    {
        /* Set the error flag so the logging system knows we had an error. */
        ctx->had_error = NX_YES;
    }

    return (0);
}

// void kill_all_children(struct output_writter *output)
// {
//     uint32_t i = 0;
//     int32_t rtrn = 0;
//
//     for(i = 0; i < total_children; i++)
//     {
//         int32_t pid = atomic_load_int32(&children[i]->pid);
//
//         rtrn = kill(pid, SIGKILL);
//         if(rtrn < 0)
//         {
//             output->write(ERROR, "Can't kill child: %s\n", strerror(errno));
//             return;
//         }
//     }
//
//     return;
// }

/**
 * This is the fuzzing loop for syscall fuzzing in dumb mode.
 */
NX_NO_RETURN static void start_syscall_child(struct thread_ctx *thread,
                                             struct memory_allocator *allocator,
                                             struct output_writter *output,
                                             struct resource_generator *rsrc_gen,
                                             struct random_generator *random)
{
    int32_t rtrn = 0;
    struct child_ctx *child = NULL;

    epoch_start(thread, allocator, output);

    /* Get our child context. */
    child = get_child(output);
    if(child == NULL)
    {
        output->write(ERROR, "Can't get child context\n");
        exit_child(thread, allocator, output);
    }

    /* Set the return jump so that we can try fuzzing again on a signal. */
    rtrn = setjmp(child->return_jump);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't set return jump\n");
        exit_child(thread, allocator, output);
    }

    /* Check to see if we jumped back from the signal handler. */
    if(atomic_load_int32(&child->did_jump) == NX_YES)
    {
        /* Start an epoch protected section. */
        epoch_start(thread, allocator, output);

        /* Log the results of the last fuzz test. */
        rtrn = log_results(child->had_error, child->ret_value, strsignal(child->sig_num));
        if(rtrn < 0)
        {
            output->write(ERROR, "Can't log test results\n");
            exit_child(thread, allocator, output);
        }

        /* Clean up our old mess. */
        rtrn = free_old_arguments(child, output, allocator, rsrc_gen);
        if(rtrn < 0)
        {
            output->write(ERROR, "Can't cleanup old arguments\n");
            exit_child(thread, allocator, output);
        }

        epoch_stop(thread, allocator);
    }

    epoch_stop(thread, allocator);

    /* Check if we should stop or continue running. */
    while(atomic_load_int32(stop) != TRUE)
    {
        epoch_start(thread, allocator, output);

        /* Generate mutated syscall arguments for a randomly chosen syscall. */
        rtrn = generate_test_case(child, thread, output, allocator, random);
        if(rtrn < 0)
        {
            output->write(ERROR, "Syscall call failed\n");
            exit_child(thread, allocator, output);
        }

        epoch_stop(thread, allocator);

        /* Run the syscall we selected with the arguments we generated and mutated. This call usually
        crashes and causes us to jumb back to the setjmp call above. */
        rtrn = test_syscall(child, output);
        if(rtrn < 0)
        {
            output->write(ERROR, "Syscall call failed\n");
            exit_child(thread, allocator, output);
        }

        rtrn = log_results(child->had_error, child->ret_value, strsignal(child->sig_num));
        if(rtrn < 0)
        {
            output->write(ERROR, "Can't log test results\n");
            exit_child(thread, allocator, output);
        }

        /* If we didn't crash, cleanup are mess. If we don't do this the generate
        functions will crash in a hard to understand way. */
        free_old_arguments(child, output, allocator, rsrc_gen);
    }

    /* We should not get here, but if we do, exit so we can be restarted. */
    exit_child(thread, allocator, output);
}

NX_NO_RETURN static void start_child_loop(struct thread_ctx *thread,
                                          struct memory_allocator *allocator,
                                          struct output_writter *output,
                                          struct resource_generator *rsrc_gen,
                                          struct random_generator *random)
{
    /* If were in dumb mode start the dumb syscall loop. */
    if(mode != TRUE)
    {
        start_syscall_child(thread, allocator, output, rsrc_gen, random);
    }

    start_smart_syscall_child(thread, allocator, output);
}

static void init_syscall_child(uint32_t i,
                               struct thread_ctx *thread,
                               struct memory_allocator *allocator,
                               struct output_writter *output)
{
    int32_t rtrn = 0;

    /* Set up the child signal handler. */
    setup_child_signal_handler(output);

    /* Start an epoch protected section. */
    if(epoch_start(thread, allocator, output) == -1)
    {
        output->write(ERROR, "Can't start epoch protected section\n");
        exit_child(thread, allocator, output);
    }

    /* Check if we are in smart mode. */
    if(mode == TRUE)
    {
        /* Inject probes into the kernel. */
        rtrn = inject_kernel_probes(children[i]->probe_handle);
        if(rtrn < 0)
        {
            output->write(ERROR, "Can't init child probes\n");
            exit_child(thread, allocator, output);
        }
    }

    /* End epoch protected section. */
    epoch_stop(thread, allocator);

    return;
}

NX_NO_RETURN static void start_child(uint32_t i,
                                     struct memory_allocator *allocator,
                                     struct output_writter *output,
                                     struct resource_generator *rsrc_gen,
                                     struct random_generator *random)
{
    struct thread_ctx *thread = NULL;

    /* Register the child's main thread with the global epoch. */
    thread = init_thread(epoch, allocator, output);
    if(thread == NULL)
    {
        output->write(ERROR, "Thread initialization failed\n");
        exit_child(thread, allocator, output);
    }

    /* Initialize the new child. */
    init_syscall_child(i, thread, allocator, output);

    /* Start the newly created syscall child's main loop. */
    start_child_loop(thread, allocator, output, rsrc_gen, random);
}

static int32_t create_child(struct thread_ctx *thread,
                            struct memory_allocator *allocator,
                            struct output_writter *output,
                            struct resource_generator *rsrc_gen,
                            struct random_generator *random)
{
    pid_t pid = 0;
    uint32_t i = 0;

    /* Walk the child structure array and find the first empty child slot. */
    for(i = 0; i < total_children; i++)
    {
        /* Start epoch protected section. */
        if(epoch_start(thread, allocator, output) == -1)
        {
            output->write(ERROR, "Can't start protected section\n");
            return (-1);
        }

        /* Check if the child pid is not set to empty, skip
          and continue if the pid is valid, ie not EMPTY. */
        if(atomic_load_int32(&children[i]->pid) != EMPTY)
        {
            /* End the epoch protected section. */
            epoch_stop(thread, allocator);
            continue;
        }

        /* End the epoch protected section. */
        epoch_stop(thread, allocator);

        /* Create a pipe so we can avoid a race on child creation.
        We don't wan't the parent to try creating a child on a child
        it's already created, but hasn't set it's pid to something not
        EMPTY. Use the pipe to message the parent when it's safe to continue. */
        int32_t fd[2];
        int32_t rtrn = pipe(fd);
        if(rtrn < 0)
        {
            output->write(ERROR, "Message pipe creation failed\n");
            return (-1);
        }

        /* Fork and create child process. Don't use the variable
        thread inside the child because it belongs to the parent.
        The child has to create it's own thread ctx. */
        pid = fork();
        if(pid == 0)
        {
            /* Set the child pid and increment the running child counter. Do this
            right away so we can let the parent continue as soon as possible. */
            cas_loop_int32(&children[i]->pid, getpid());
            atomic_add_uint32(&state->running_children, 1);

            /* Let the parent process know it's safe to continue. */
            ssize_t ret = write(fd[1], "!", 1);
            if(ret < 1)
            {
                output->write(ERROR, "Write: %s\n", strerror(errno));
                exit_child(thread, allocator, output);
            }

            /* Start child process's loop. */
            start_child(i, allocator, output, rsrc_gen, random);
        }
        else if(pid > 0)
        {
            char *buf[1] = {0};

            /* Wait for a byte from the child saying it's safe to return. */
            ssize_t ret = read(fd[0], buf, 1);
            if(ret < 1)
            {
                output->write(ERROR, "Read: %s\n", strerror(errno));
                return (-1);
            }

            /* Clean up pipe descriptors. */
            (void)close(fd[0]);
            (void)close(fd[1]);

            return (0);
        }
        else
        {
            output->write(ERROR, "Can't create child proc: %s\n", strerror(errno));
            return (-1);
        }
    }

    return (0);
}

void start_main_syscall_loop(struct thread_ctx *thread,
                             struct memory_allocator *allocator,
                             struct output_writter *output,
                             struct resource_generator *rsrc_gen,
                             struct random_generator *random)
{
    output->write(STD, "Starting fuzzer\n");

    int32_t rtrn = 0;

    /* Set up signal handler. */
    rtrn = setup_signal_handler(output);
    if(rtrn < 0)
    {
        output->write(ERROR, "Signal handler setup failed\n");
        return;
    }

    /* Check if we should stop or continue running. */
    while(atomic_load_int32(stop) == FALSE)
    {
        /* Check if we have the right number of children processes running, if not create a new ones until we do. */
        if(atomic_load_uint32(&state->running_children) < total_children)
        {
            /* Create children process. */
            rtrn = create_child(thread, allocator, output, rsrc_gen, random);
            if(rtrn < 0)
            {
                output->write(ERROR, "Can't create child\n");
                return;
            }
        }
    }

    output->write(STD, "Exiting main loop\n");
}

static struct child_ctx *init_child_context(struct output_writter *output, struct memory_allocator *allocator)
{
    struct child_ctx *child = NULL;

    /* Allocate the child context object. */
    child = allocator->shared(sizeof(struct child_ctx));
    if(child == NULL)
    {
        output->write(ERROR, "Can't create child context\n");
        return (NULL);
    }

    /* Set current arg to zero. */
    child->current_arg = 0;

    /* Create the index where we store the syscall arguments. */
    child->arg_value_array = allocator->shared(ARG_LIMIT * sizeof(uint64_t *));
    if(child->arg_value_array == NULL)
    {
        output->write(ERROR, "Can't create arg value index: %s\n", strerror(errno));
        allocator->free_shared((void **)&child, sizeof(struct child_ctx));
        return (NULL);
    }

    /* This index tracks the size of the argument generated.  */
    child->arg_size_array = allocator->shared(ARG_LIMIT * sizeof(uint64_t));
    if(child->arg_size_array == NULL)
    {
        output->write(ERROR, "Can't create arg size index: %s\n", strerror(errno));
        allocator->free_shared((void **)&child->arg_value_array, ARG_LIMIT * sizeof(uint64_t *));
        allocator->free_shared((void **)&child, sizeof(struct child_ctx));
        return (NULL);
    }

    child->arg_copy_array = allocator->shared(ARG_LIMIT * sizeof(uint64_t *));
    if(child->arg_copy_array == NULL)
    {
        output->write(ERROR, "Can't create arg copy index: %s\n", strerror(errno));
        allocator->free_shared((void **)&child->arg_size_array, ARG_LIMIT * sizeof(uint64_t));
        allocator->free_shared((void **)&child->arg_value_array, ARG_LIMIT * sizeof(uint64_t *));
        allocator->free_shared((void **)&child, sizeof(struct child_ctx));
        return (NULL);
    }

    uint32_t i = 0;

    /* Loop and create the various arrays in the child struct. */
    for(i = 0; i < ARG_LIMIT; i++)
    {
        child->arg_value_array[i] = allocator->shared(ARG_BUF_LEN);
        if(child->arg_value_array[i] == NULL)
        {
            output->write(ERROR, "Can't create arg value\n");
            /* Still potentially leaking memory from the indicies
             in the arg_value_array, ie if malloc fails on the second
             or higher iteration. */
            allocator->free_shared((void **)&child->arg_copy_array, ARG_LIMIT * sizeof(uint64_t *));
            allocator->free_shared((void **)&child->arg_size_array, ARG_LIMIT * sizeof(uint64_t));
            allocator->free_shared((void **)&child->arg_value_array, ARG_LIMIT * sizeof(uint64_t *));
            allocator->free_shared((void **)&child, sizeof(struct child_ctx));
            return (NULL);
        }

        child->arg_copy_array[i] = allocator->shared(ARG_BUF_LEN);
        if(child->arg_copy_array[i] == NULL)
        {
            output->write(ERROR, "Can't create arg copy value\n");
            /* Still potentially leaking memory from the indicies
             in the arg_copy_array, ie if malloc fails on the second
             or higher iteration. */
            allocator->free_shared((void **)&child->arg_copy_array, ARG_LIMIT * sizeof(uint64_t *));
            allocator->free_shared((void **)&child->arg_size_array, ARG_LIMIT * sizeof(uint64_t));
            allocator->free_shared((void **)&child->arg_value_array, ARG_LIMIT * sizeof(uint64_t *));
            allocator->free_shared((void **)&child, sizeof(struct child_ctx));
            return (NULL);
        }
    }

    return (child);
}

int32_t setup_syscall_module(int32_t *stop_ptr,
                             uint32_t *counter,
                             int32_t run_mode,
                             epoch_ctx *e,
                             struct memory_allocator *allocator,
                             struct output_writter *output)
{
    uint32_t i = 0;
    int32_t rtrn = 0;

    /* Grab the core count of the machine we are on and set the number
    of syscall children to the core count. */
    rtrn = get_core_count(&total_children);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't get core count\n");
        return (-1);
    }

    /* Grab the syscall table for the system we are on. */
    sys_table = get_syscall_table(output, allocator);
    if(sys_table == NULL)
    {
        output->write(ERROR, "Can't get syscall table\n");
        return (-1);
    }

    state = allocator->shared(sizeof(struct global_state));
    if(state == NULL)
    {
        output->write(ERROR, "Global state allocation failed\n");
        return (-1);
    }

     /* Set running children to zero. */
    atomic_store_uint32(&state->running_children, 0);

    /* Create the child process structures. */
    children = allocator->shared(total_children * sizeof(struct child_ctx *));
    if(children == NULL)
    {
        output->write(ERROR, "Can't create children index.\n");
        return (-1);
    }

    /* Loop for each child and allocate the child context object. */
    for(i = 0; i < total_children; i++)
    {
        struct child_ctx *child = NULL;

        /* Create and initialize the child context struct. */
        child = init_child_context(output, allocator);
        if(child == NULL)
        {
            output->write(ERROR, "Can't init child context\n");
            return (-1);
        }

        /* Set the newly created child context to the children index. */
        children[i] = child;
    }

    /* Set file scope variables. */
    stop = stop_ptr;
    mode = run_mode;
    epoch = e;
    state->test_counter = counter;

    /* Now set the table set flag. */
    table_set = TRUE;

    return (0);
}

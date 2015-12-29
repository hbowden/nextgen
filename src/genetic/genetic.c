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

#include "genetic.h"
#include "syscall/syscall.h"
#include "memory/memory.h"
#include "crypto/crypto.h"
#include "job.h"
#include "io/io.h"

#include <dtrace.h>
#include <errno.h>
#include <string.h>

#include "stdatomic.h"

#define SPECIES_POP 1000

static atomic_int_fast32_t *stop;

static msg_port_t msg_port;

static enum genetic_mode run_mode;

struct chromosome_ctx
{
    int32_t *encoding;
};

struct organism_ctx
{
    double fitness;

    struct chromosome_ctx *chromosome;

    NX_SLIST_ENTRY(organism_ctx);
};

struct species_ctx
{
    const char *species_name;

    uint32_t species_population;

    double average_species_fitness;

    NX_SLIST_HEAD(organism_list, organism_ctx);
};

struct world_population
{
    uint32_t number_of_species;

    uint64_t current_generation;

    struct species_ctx **species;

    double average_fitness;
};

static struct world_population *world;

static int32_t init_world(void)
{
    output(STD, "Creating world, may take a sec\n");

    /* Allocate the world struct. */
    world = mem_alloc(sizeof(struct world_population));
    if(world == NULL)
    {
        output(ERROR, "Can't allocate world\n");
        return (-1);
    }

    /* Grab a copy of the syscall table. */
    struct syscall_table_shadow *sys_table = get_syscall_table();
    if(sys_table == NULL)
    {
        output(ERROR, "Can't get system table\n");
        return (-1);
    }

    /* Set the number of species to the number of syscalls. */
    world->number_of_species = sys_table->number_of_syscalls;

    /* Set current generation to zero because we haven't created one yet. */
    world->current_generation = 0;

    /* Allocate index of species context pointers. */
    world->species = mem_alloc((world->number_of_species) * sizeof(struct species_ctx *));
    if(world->species == NULL)
    {
        output(ERROR, "Can't create species index\n");
        return (-1);
    }

    uint32_t i;

    for(i = 0; i < world->number_of_species; i++)
    {
        struct species_ctx ctx = {

            /* Set species name to syscall name. */
            .species_name = sys_table->sys_entry[i]->name_of_syscall,

            /* Set population to zero. */
            .species_population = 0,

            /* Set fitness to zero. */
            .average_species_fitness = 0,

            /* Init the organism list. */
            .organism_list = SLIST_HEAD_INITIALIZER(ctx ->organism_list)

        };

        struct species_ctx *specie = NULL;

        specie = mem_alloc(sizeof(struct species_ctx));
        if(specie == NULL)
        {
            output(ERROR, "Can't allocate species_ctx\n");
            return -1;
        }

        memmove(specie, &ctx, sizeof(struct species_ctx));

        uint32_t ii;

        for(ii = 0; ii < SPECIES_POP; ii++)
        {
            struct organism_ctx *organism = NULL;

            organism = mem_alloc(sizeof(struct organism_ctx));
            if(organism == NULL)
            {
                output(ERROR, "Can't allocate organism context\n");
                return (-1);
            }

            organism->chromosome = mem_alloc(sizeof(struct chromosome_ctx));
            if(organism->chromosome == NULL)
            {
                output(ERROR, "Can't allocate chromosome context\n");
                return (-1);
            }

            organism->fitness = 0;

            CK_SLIST_INSERT_HEAD(&specie->organism_list, organism, list_entry);
        }

        world->species[i] = specie;
    }

    return (0);
}

static struct job_ctx *create_job(enum job_type type, uint32_t syscall_number)
{
    struct job_ctx *job = NULL;

    /* Allocate job context. */
    job = mem_alloc(sizeof(struct job_ctx));
    if(job == NULL)
    {
        output(ERROR, "Can't allocate job context\n");
        return NULL;
    }

    /* Set job values. */
    job->type = type;
    job->syscall_number = syscall_number;

    return (job);
}

static int32_t submit_job(struct job_ctx *job)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    /* Randomly pick a number. */
    rtrn = rand_range(number_of_children, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    struct child_ctx *child = NULL;

    /* Grab the child context of the randomly selected child process. */
    child = get_child_from_index(number);
    if(child == NULL)
    {
        output(ERROR, "Can't get child context\n");
        return (-1);
    }

    /* Send the job to the child's message port for processing. */
    msg_send(msg_port, child->msg_port, job);

    return (0);
}


static int32_t create_first_generation(void)
{
    output(STD, "Creating first generation\n");

    /* Grab a copy of the syscall table. */
    struct syscall_table_shadow *sys_table = get_syscall_table();
    if(sys_table == NULL)
    {
        output(ERROR, "Can't get system table\n");
        return (-1);
    }

    uint32_t i, ii;
    int32_t rtrn = 0;
    uint32_t syscall_symbol = 0;

    /* Loop for each syscall in the syscall table. */
    for(i = 0; i < sys_table->number_of_syscalls; i++)
    {
        for(ii = 0; ii < SPECIES_POP; ii++)
        {
            struct job_ctx *job = NULL;

            syscall_symbol = sys_table->sys_entry[i]->syscall_symbol;

            job = create_job(GENESIS, syscall_symbol);
            if(job == NULL)
            {
                output(ERROR, "Can't create job\n");
                return (-1);
            }

            rtrn = submit_job(job);
            if(rtrn < 0)
            {
                output(ERROR, "Can't submit job\n");
                return (-1);
            }
        }
    }

    return (0);
}

static int32_t genesis(void)
{
    int32_t rtrn = 0;

    /* Allocate and initialize all the data structures needed by the genetic alorithm. */
    rtrn = init_world();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init world\n");
        return (-1);
    }

    /* Create the first generation of organisms aka test cases. */
    rtrn = create_first_generation();
    if(rtrn < 0)
    {
        output(ERROR, "Can't create first generation\n");
        return (-1);
    }

    return (0);
}

static void god_loop(void)
{
    int32_t rtrn = 0;

    /* genesis() init's all the needed data structures and creates the first population. */
    rtrn = genesis();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init ga\n");
        return;
    }

    /* Start main loop for the genetic algorithm.
    Each loop creates a new generation. */
    while(atomic_load(stop) != TRUE)
    {
        
    }
    
    return;
}

int32_t setup_genetic_module(enum genetic_mode mode, pid_t *pid, atomic_int_fast32_t *stop_ptr)
{
    int32_t rtrn = 0;
    pid_t god_pid = 0;

    run_mode = mode;
    stop = stop_ptr;

    /* Fork and create the genetic algo process. */
    god_pid = fork();
    if(god_pid == 0)
    {
        (*pid) = getpid();

        /* Start main genetic algo loop. */
    	god_loop();

        /* Exit and cleanup. */
        _exit(0);
    }
    else if(god_pid > 0)
    {
    	return (0);
    }
    else
    {
    	output(ERROR, "Failed to fork god process: %s\n", strerror(errno));
    	return (-1);
    }
}



/**
 * Copyright (c) 2015, Harrison Bowden, Secure Labs, Minneapolis, MN
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
#include "nextgen.h"
#include "syscall.h"
#include "memory.h"
#include "io.h"

#include <dtrace.h>
#include <errno.h>
#include <string.h>

#include "stdatomic.h"

#define SPECIES_POP 1000

struct chromosome_ctx
{
    int32_t *encoding;
};

struct organism_ctx
{
    double fitness;

    struct chromosome_ctx *chromosome;

    CK_SLIST_ENTRY(organism_ctx) list_entry;
};

struct species_ctx
{
    const char *species_name;

    uint32_t species_population;

    double average_species_fitness;

    CK_SLIST_HEAD(organism_list, organism_ctx) organism_list;
};

struct world_population
{
    uint32_t number_of_species;

    uint64_t current_generation;

    struct species_ctx **species;

    double average_fitness;
};

static struct world_population *world;

int create_new_generation(char **file, off_t *file_size, char *file_extension)
{
	
	return 0;
}

static int32_t init_world(void)
{
    output(STD, "Creating world, may take a sec\n");

    /* Allocate the world struct. */
    world = mem_alloc(sizeof(struct world_population));
    if(world == NULL)
    {
        output(ERROR, "Can't allocate world\n");
        return -1;
    }

    /* Grab a copy of the syscall table. */
    struct syscall_table_shadow *sys_table = get_syscall_table();
    if(sys_table == NULL)
    {
        output(ERROR, "Can't get system table\n");
        return -1;
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
        return -1;
    }

    uint32_t i;

    for(i = 0; i < world->number_of_species; i++)
    {
        struct species_ctx ctx = {

            /* Set species name to syscall name. */
            .species_name = sys_table->sys_entry[i].name_of_syscall,

            /* Set population to zero. */
            .species_population = 0,

            /* Set fitness to zero. */
            .average_species_fitness = 0,

            /* Init the organism list. */
            .organism_list = CK_SLIST_HEAD_INITIALIZER(ctx ->organism_list)

        };

        struct species_ctx *specie = &ctx;

        specie = mem_alloc(sizeof(struct species_ctx));
        if(specie == NULL)
        {
            output(ERROR, "Can't allocate species_ctx\n");
            return -1;
        }

        uint32_t ii;

        for(ii = 0; ii < SPECIES_POP; ii++)
        {
            struct organism_ctx *organism = NULL;

            organism = mem_alloc(sizeof(struct organism_ctx));
            if(organism == NULL)
            {
                output(ERROR, "Can't allocate organism context\n");
                return -1;
            }

            organism->chromosome = mem_alloc(sizeof(struct chromosome_ctx));
            if(organism->chromosome == NULL)
            {
                output(ERROR, "Can't allocate chromosome context\n");
                return -1;
            }

            organism->fitness = 0;

            CK_SLIST_INSERT_HEAD(&specie->organism_list, organism, list_entry);
        }
    }

    return 0;
}

struct job_ctx *get_job(struct work_queue *queue)
{
    /* Return a job. */
    return (struct job_ctx *)(get_from_queue(queue));
}

static int32_t submit_job(uint32_t number, struct job_ctx *job)
{
    uint32_t i = 0;
    int32_t rtrn = 0;

    /* Make sure zero is not passed or that job is not NULL. */
    if(number == 0 || job == NULL)
        return (-1);

    /* Loop number times. */
    for(i = 0; i < number; i++)
    {
        struct queue_block *q_blk = NULL;

        q_blk = get_queue_block(map->queue);
        if(q_blk == NULL)
        {
            output(ERROR, "Can't get queue block\n");
            return (-1);
        }

        struct job_ctx *job_object = (struct job_ctx *)q_blk->ptr;

        /* Set the job type to genesis. */
        job_object->type = job->type;

        /* Set the syscall to test. */
        job_object->syscall_symbol = job->syscall_symbol;

        /* Set the number of args. */
        job_object->number_of_args = job->number_of_args;

        /* Insert the job into the work queue. */
        rtrn = insert_into_queue(q_blk, map->queue);
        if(rtrn < 0)
        {
            output(ERROR, "Can't insert job into queue\n");
            return (-1);
        }
    }

    return (0);
}

static int create_first_generation(void)
{
    output(STD, "Creating first generation\n");

    /* Grab a copy of the syscall table. */
    struct syscall_table_shadow *sys_table = get_syscall_table();
    if(sys_table == NULL)
    {
        output(ERROR, "Can't get system table\n");
        return (-1);
    }

    uint32_t i;
    int32_t rtrn = 0;

    for(i = 0; i < sys_table->number_of_syscalls; i++)
    {
        struct job_ctx *job = NULL;

        job = mem_alloc_shared(sizeof(struct job_ctx));
        if(job == NULL)
        {
            output(ERROR, "Can't allocate job\n");
            return (-1);
        }

        /* Set the job type to genesis. */
        job->type = GENESIS;

        /* Set the syscall to test. */
        job->syscall_symbol = sys_table->sys_entry[i].syscall_symbol;

        /* Set the number of args. */
        job->number_of_args = sys_table->sys_entry[i].number_of_args;

        /* Submit one thousand jobs for this syscall. */
        rtrn = submit_job(1000, job);
        if(rtrn < 0)
        {
            output(ERROR, "Can't submit job\n");
            return -1;
        }
    }

    return 0;
}

static int genesis(void)
{
    int32_t rtrn = 0;

    /* Allocate and initialize all the data structures needed by the genetic alorithm. */
    rtrn = init_world();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init world\n");
        return -1;
    }

    /* Create the first generation of organisms aka test cases. */
    rtrn = create_first_generation();
    if(rtrn < 0)
    {
        output(ERROR, "Can't create first generation\n");
        return -1;
    }

    return 0;
}

static void god_loop(void)
{
    int32_t rtrn = 0;

    /* Start main loop for the genetic algorithm.
    Each loop creates a new generation. */
    while(atomic_load(&map->stop) != TRUE)
    {
        
    }
    
    return;
}

int32_t setup_genetic_module(void)
{
    int32_t rtrn = 0;
    pid_t god_pid = 0;
    
    /* Initialize the global work queue. */
    rtrn = create_work_queue(map->queue, sizeof(struct job_ctx));
    if(rtrn < 0)
    {
        output(ERROR, "Can't create work queue\n");
        return (-1);
    }

    /* genesis() init's all the needed data structures and creates the first population. */
    rtrn = genesis();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init ga\n");
        return (-1);
    }

    /* Fork and create the genetic algo process. */
    god_pid = fork();
    if(god_pid == 0)
    {
        /* Set the PID in the global map. */
        cas_loop_int32(&map->god_pid, getpid());

        /* Start main genetic algo loop. */
    	god_loop();

        /* Exit and cleanup. */
        _exit(0);
    }
    else if(god_pid > 0)
    {
    	return 0;
    }
    else
    {
    	output(ERROR, "Failed to fork god process: %s\n", strerror(errno));
    	return -1;
    }
}

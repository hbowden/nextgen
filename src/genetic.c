

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
#include "memory.h"
#include "concurrent.h"
#include "io.h"

#include <dtrace.h>
#include <errno.h>

#include "stdatomic.h"

#define SPECIES_POP 1000

struct chromosome_ctx
{
    int *encoding;
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

    unsigned int species_population;

    double average_species_fitness;

    CK_SLIST_HEAD(organism_list, organism_ctx) organism_list;
};

struct world_population
{
    unsigned int number_of_species;

    unsigned int current_generation;

    struct species_ctx **species;

    double average_fitness;
};

static struct world_population *world;

int create_new_generation(char **file, off_t *file_size, char *file_extension)
{
	
	return 0;
}

static int init_world(void)
{
    world = mem_alloc(sizeof(struct world_population));
    if(world == NULL)
    {
        output(ERROR, "Can't allocate world\n");
        return -1;
    }

    /* Set the number of species to the number of syscalls. */
    world->number_of_species = map->sys_table->number_of_syscalls;

    /* Set current generation to zero because we haven't created one yet. */
    world->current_generation = 0;

    /* Allocate index of species context pointers. */
    world->species = mem_alloc((sizeof(struct species_ctx *)) * world->number_of_species);
    if(world->species == NULL)
    {
        output(ERROR, "Can't create species index\n");
        return -1;
    }

    unsigned int i;

    for(i = 0; i < world->number_of_species; i++)
    {
        struct species_ctx ctx = {

            /* Set species name to syscall name. */
            .species_name = map->sys_table->sys_entry[i].name_of_syscall,

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

        unsigned int ii;

        for(ii = 0; ii < SPECIES_POP; i++)
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

static int create_first_generation(void)
{
    return 0;
}

static int genesis(void)
{
    int rtrn;

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
        output(ERROR, "Can't craete first generation\n");
        return -1;
    }

    return 0;
}

static void god_loop(void)
{
    int rtrn;

    /* genesis() init's all the needed data structures and creates the first population. */
    rtrn = genesis();
    if(rtrn < 0)
    {
        output(ERROR, "Can't init ga\n");
        return;
    }

    /* Start main loop for the genetic algorithm. */
    while(atomic_load(&map->stop) != TRUE)
    {

    }
    
    return;
}

int start_god(void)
{
    pid_t god_pid;

    god_pid = fork();
    if(god_pid == 0)
    {
        cas_loop_int32(&map->god_pid, getpid());

    	god_loop();
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

	return 0;
}

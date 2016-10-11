/**
 * Copyright (c) 2016, Harrison Bowden, Minneapolis, MN
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

#include "runtime/nextgen.h"
#include "runtime/runtime.h"
#include "memory/memory.h"
#include "objc/objc-utils.h"
#include "io/io.h"

int main(int argc, const char * argv[])
{
    int32_t rtrn = 0;
    struct parser_ctx *ctx = NULL;
    //struct fuzzer *nextgen = NULL;
    struct output_writter *output = NULL;

    output = get_console_writter();
    if(output == NULL)
    {
        /* Default to the console if we fail to create a output writter. */
        printf("Failed to get console writter\n");
        return (-1);
    }

    // /* Get selected fuzzer. */
    // nextgen = get_fuzzer(map, output);
    // if(nextgen == NULL)
    // {
    //     output->write(ERROR, "Failed to initialize fuzzer");
    //     return (-1);
    // }

    /* Create a shared memory map so that we can share state with other threads and processes. */
    map = mem_alloc_shared(sizeof(struct shared_map));
    if(map == NULL)
    {
        output->write(ERROR, "Can't create shared object.\n");
        return (-1);
    }

    /* Parse the command line for user input. parse_cmd_line() will set variables
    in map, the shared memory mapping. This will tell the fuzzer how to run. */
    ctx = parse_cmd_line(argc, argv);
    if(ctx == NULL)
    {
        output->write(ERROR, "Can't parse command line.\n");
        return (-1);
    }

    /* Make sure we have root. */
    rtrn = check_root();
    if(rtrn != 0)
    {
        output->write(STD, "Run nextgen as root\n");
        return (-1);
    }

    /* Setup the shared map now that we got our options from the command line. */
    rtrn = init_shared_mapping(&map, ctx);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't initialize.\n");
        return (-1);
    }

    /* If we are file fuzzing on OSX we need to use objective c
    and as a consequence we need to setup the objc run loop. */
    if(map->mode == MODE_FILE)
    {
        /* Pass the objective c runtime our setup and start functions. */
        rtrn = setup_objc_runtime((int32_t (*)(void *))&setup_runtime,
                                  &start_runtime, map);
        if(rtrn < 0)
        {
           output->write(ERROR, "Can't setup objective-c runtime\n");
           return (-1);
        }
    }
    else
    {
        /* Setup the fuzzer running enviroment. */
        rtrn = setup_runtime(map);
        if(rtrn < 0)
        {
            output->write(ERROR, "Can't setup runtime enviroment.\n");
            return (-1);
        }

        /* Start the main fuzzing loop. */
        rtrn = start_runtime();
        if(rtrn < 0)
        {
            output->write(ERROR, "Can't start runtime enviroment.\n");
            clean_shared_mapping();
            return (-1);
        }
    }

    /* We should only reach here on ctrl-c. */
    clean_shared_mapping();

    return (0);
}

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

#include "nextgen.h"
#include "memory/memory.h"
#include "io/io.h"

/**
 * Main is the entry point to nextgen. In main we check for root, unfortunetly we need root to execute.
 * This is because we have to use dtrace, as well as bypass sandboxes, inject code into processes and 
 * other activities that require root access. We then create shared memory so we can share information
 * between processes. Next we parse the command line for user arguments and we stick the results in the
 * shared memory map. After parsing we set up the enviroment to the user's specfications and then finnaly
 * start the runtime, ie start fuzzing.
 **/
int main(int argc, const char * argv[])
{
    int32_t rtrn = 0;
    struct parser_ctx *ctx = NULL;

    /* Create a shared memory map so that we can share state with other threads and procceses. */
    map = mem_alloc_shared(sizeof(struct shared_map));
    if(map == NULL)
    {
        output(ERROR, "Can't create shared object.\n");
        return (-1);
    }

    /* Parse the command line for user input. parse_cmd_line() will set variables
    in map, the shared memory mapping. This will tell the fuzzer how to run. */
    ctx = parse_cmd_line(argc, argv);
    if(ctx == NULL)
    {
        output(ERROR, "Can't parse command line.\n");
        return (-1);
    }

    rtrn = check_root();
    if(rtrn != 0)
    {
        output(STD, "Run nextgen as root\n");
        return (-1);
    }

    /* Setup the shared map now that we got our options from the command line. */
    rtrn = init_shared_mapping(&map, ctx);
    if(rtrn < 0)
    {
        output(ERROR, "Can't initialize.\n");
        return (-1);
    }

    if(map->mode == MODE_FILE)
    {
        rtrn = setup_objc_runtime();
        if(rtrn < 0)
        {
           output(ERROR, "Can't setup objective-c runtime\n");
           return (-1);
        }
    }
    else
    {
        /* Setup the fuzzer running enviroment. */
        rtrn = setup_runtime();
        if(rtrn < 0)
        {
            output(ERROR, "Can't setup runtime enviroment.\n");
            clean_shared_mapping();
            return (-1);
        }

        /* Start the main fuzzing loop. */
        rtrn = start_runtime();
        if(rtrn < 0)
        {
            output(ERROR, "Can't start runtime enviroment.\n");
            clean_shared_mapping();
            return (-1);
        }
    }

    /* We should only reach here on ctrl-c. */
    clean_shared_mapping();

    return (0);
}

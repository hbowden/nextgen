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

#ifdef LINUX

/* We need to define _GNU_SOURCE to use
 asprintf on Linux. We also need to place
 _GNU_SOURCE at the top of the file before
 any other includes for it to work properly. */
#define _GNU_SOURCE

#endif

#include "nextgen.h"
#include "crypto/crypto.h"
#include "file/file.h"
#include "io/io.h"
#include "memory/memory.h"
#include "plugins/plugin.h"
#include "resource/resource.h"

#include <errno.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/stat.h>

struct shared_map *map;

static const char *optstring = "p:e:";

static struct option longopts[] = {{"in", required_argument, NULL, 'i'},
                                   {"out", required_argument, NULL, 'o'},
                                   {"exec", required_argument, NULL, 'e'},
                                   {"port", required_argument, NULL, 'p'},
                                   {"address", required_argument, NULL, 'a'},
                                   {"protocol", required_argument, NULL, 'c'},
                                   {"args", required_argument, NULL, 'x'},
                                   {"file", 0, NULL, 'f'},
                                   {"network", 0, NULL, 'n'},
                                   {"syscall", 0, NULL, 's'},
                                   {"help", 0, NULL, 'h'},
                                   {"dumb", 0, NULL, 'd'},
                                   {NULL, 0, NULL, 0}};

static void display_help_banner(struct output_writter *output)
{
    output->write(STD, "Nextgen is a Genetic File, Syscall, and Network Fuzzer.\n");
    output->write(STD, "To use the file fuzzer in smart mode run the command below.\n");
    output->write(STD, "sudo ./nextgen --file --in /path/to/in/directory --out "
                "/path/to/out/directory --exec /path/to/target/exec .\n");
    output->write(STD, "To use the syscall fuzzer in smart mode run.\n");
    output->write(STD, "sudo ./nextgen --syscall --out /path/to/out/directory\n");
    output->write(STD, "To use dumb mode just pass --dumb with any of the above commands.\n");

    return;
}

static struct fuzzer_config *init_fuzzer_config(struct output_writter *output,
                                                struct memory_allocator *allocator)
{
    struct fuzzer_config *config = NULL;

    config = allocator->alloc(sizeof(struct fuzzer_config));
    if(config == NULL)
    {
        output->write(ERROR, "Failed to allocate fuzzer config\n");
        return (NULL);
    }

    config->exec_path = NULL;
    config->input_path = NULL;
    config->output_path = NULL;
    config->args = NULL;

    return (config);
}

static int32_t set_input_path(struct fuzzer_config *config,
                              char *path,
                              struct output_writter *output)
{
    /* Make sure we have a valid parser context. If
    not return an error and print to stderr. */
    if(config == NULL)
    {
        output->write(ERROR, "Parser context is not allocated\n");
        return (-1);
    }

    /* The path supplied by the caller should be non NULL. */
    if(path == NULL)
    {
        output->write(ERROR, "Path supplied is NULL\n");
        return (-1);
    }

    struct stat sb;
    int32_t rtrn = 0;

    /* Get filesystem stats for the path supplied. */
    rtrn = stat(path, &sb);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't get stats: %s\n", strerror(errno));
        return(-1);
    }

    /* Make sure path is a directory. */
    if(sb.st_mode & S_IFDIR)
    {
        /* We were passed a directory so set the input path in the context. */
        config->input_path = path;
        return (0);
    }
    else
    {
        output->write(ERROR, "Input path is not a directory\n");
        return (-1);
    }
}

static int32_t set_output_path(struct fuzzer_config *config,
                               char *path,
                               struct output_writter *output)
{
    /* Make sure we have a valid parser context. If
    not return an error and print to stderr. */
    if(config == NULL)
    {
        output->write(ERROR, "Parser context is not allocated\n");
        return (-1);
    }

    /* The path supplied by the caller should be non NULL. */
    if(path == NULL)
    {
        output->write(ERROR, "Path supplied is NULL\n");
        return (-1);
    }

    struct stat sb;
    int32_t rtrn = 0;

    rtrn = stat(path, &sb);
    if(rtrn != -1)
    {
        output->write(ERROR, "Path already exist\n");
        return (-1);
    }

    config->output_path = path;

    return (0);
}

static int32_t set_exec_path(struct fuzzer_config *config,
                             char *path,
                             struct output_writter *output)
{
    /* Make sure we have a valid parser context. If
    not return an error and print to stderr. */
    if(config == NULL)
    {
        output->write(ERROR, "Parser context is not allocated\n");
        return (-1);
    }

    /* The path supplied by the caller should be non NULL. */
    if(path == NULL)
    {
        output->write(ERROR, "Path supplied is NULL\n");
        return (-1);
    }

    struct stat sb;
    int32_t rtrn = 0;

    /* Get filesystem stats for the path supplied. */
    rtrn = stat(path, &sb);
    if(rtrn < 0)
    {
        output->write(ERROR, "Can't get stats: %s\n", strerror(errno));
        return(-1);
    }

    /* Make sure the path is an executable. */
    if(sb.st_mode & S_IXUSR)
    {
        /* We were passed a excutable so set the exec path in the context. */
        config->exec_path = path;
        return (0);
    }
    else
    {
        output->write(ERROR, "Exec path is not a excutable\n");
        return (-1);
    }
}

static int32_t set_fuzz_mode(struct fuzzer_config *config,
                             enum fuzz_mode mode,
                             struct output_writter *output)
{
    /* Make sure the mode passed is legit. */
    switch((int32_t)mode)
    {
        case MODE_FILE:
        case MODE_SYSCALL:
        case MODE_NETWORK:
            break;

        default:
            output->write(ERROR, "Unknown fuzz mode\n");
            return (-1);
    }

    /* Set fuzz mode to mode passed by user. */
    config->mode = mode;

    return (0);
}

struct fuzzer_config *parse_cmd_line(int32_t argc, char *argv[],
                                     struct output_writter *output,
                                     struct memory_allocator *allocator)
{
    if(argc == 1)
    {
        display_help_banner(output);
        return (NULL);
    }

    int32_t ch = 0;
    int32_t rtrn = 0;
    struct fuzzer_config *config = NULL;
    int32_t iFlag = FALSE, oFlag = FALSE, fFlag = FALSE, nFlag = FALSE,
            eFlag = FALSE, pFlag = FALSE, aFlag = FALSE, tFlag = FALSE,
            sFlag = FALSE;

    /* Allocate the parser context. */
    config = init_fuzzer_config(output, allocator);
    if(config == NULL)
    {
        output->write(ERROR, "Can't init parser context\n");
        return (NULL);
    }

    /* Default to smart_mode. */
    config->smart_mode = TRUE;

    /* Parse the command line. */
    while((ch = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
    {
        switch(ch)
        {
            /* Display banner and exit. */
            case 'h':
                display_help_banner(output);
                allocator->free((void **)&config);
                return (NULL);

            case 'p':
                pFlag = TRUE;
                (void)pFlag;
                break;

            case 'a':
                aFlag = TRUE;
                break;

            case 't':
                tFlag = TRUE;
                break;

            /* Set the path of the executable to fuzz test. */
            case 'e':
                rtrn = set_exec_path(config, optarg, output);
                if(rtrn < 0)
                {
                    output->write(ERROR, "Can't set exec path\n");
                    allocator->free((void **)&config);
                    return (NULL);
                }

                eFlag = TRUE;
                break;

            /* Set the directory path of input files. */
            case 'i':
                rtrn = set_input_path(config, optarg, output);
                if(rtrn < 0)
                {
                    output->write(ERROR, "Can't set input path\n");
                    allocator->free((void **)&config);
                    return (NULL);
                }

                iFlag = TRUE;
                break;

            /* Set the output path of where the user wan't us to
            create a output directory to store our output data. */
            case 'o':
                rtrn = set_output_path(config, optarg, output);
                if(rtrn < 0)
                {
                    output->write(ERROR, "Can't set output path\n");
                    allocator->free((void **)&config);
                    return (NULL);
                }

                oFlag = TRUE;
                break;

            case 'f':
                fFlag = TRUE;
                rtrn = set_fuzz_mode(config, MODE_FILE, output);
                if(rtrn < 0)
                {
                    output->write(ERROR, "Can't set fuzz mode\n");
                    allocator->free((void **)&config);
                    return (NULL);
                }
                break;

            case 'n':
                nFlag = TRUE;
                rtrn = set_fuzz_mode(config, MODE_NETWORK, output);
                if(rtrn < 0)
                {
                    output->write(ERROR, "Can't set fuzz mode\n");
                    allocator->free((void **)&config);
                    return (NULL);
                }
                break;

            case 's':
                sFlag = TRUE;
                rtrn = set_fuzz_mode(config, MODE_SYSCALL, output);
                if(rtrn < 0)
                {
                    output->write(ERROR, "Can't set fuzz mode\n");
                    allocator->free((void **)&config);
                    return (NULL);
                }
                break;

            case 'd':
                config->smart_mode = FALSE;
                break;

            case 'x':
                rtrn = asprintf(&config->args, "%s", optarg);
                if(rtrn < 0)
                {
                    output->write(ERROR, "asprintf: %s\n", strerror(errno));
                    allocator->free((void **)&config);
                    return (NULL);
                }
                break;

            default:
                display_help_banner(output);
                allocator->free((void **)&config);
                return (NULL);
        }
    }

    /* Make sure a fuzzing mode was selected. */
    if(fFlag != TRUE && nFlag != TRUE && sFlag != TRUE)
    {
        display_help_banner(output);
        allocator->free((void **)&config);
        return (NULL);
    }

    /* If file mode was selected lets make sure all the right args were passed.*/
    if(fFlag == TRUE)
    {
        if(iFlag == FALSE || oFlag == FALSE || eFlag == FALSE)
        {
            output->write(STD, "Pass --exec , --in and --out for file mode\n");
            allocator->free((void **)&config);
            return (NULL);
        }
    }

    if(nFlag == TRUE)
    {
        if(aFlag == FALSE || tFlag == FALSE || oFlag == FALSE)
        {
            output->write(STD, "Pass --address , --port, --protocol, and --out for "
                        "network mode\n");
            allocator->free((void **)&config);
            return (NULL);
        }
    }

    /* Check to see if syscall mode was selected. */
    if(sFlag == TRUE)
    {
        /* Make sure the user set an output path. */
        if(oFlag != TRUE)
        {
            output->write(STD, "Pass --out for syscall mode\n");
            allocator->free((void **)&config);
            return (NULL);
        }
    }

    return (config);
}

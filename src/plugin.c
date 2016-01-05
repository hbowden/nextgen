

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

#include "plugin.h"
#include "nextgen.h"
#include "memory.h"
#include "io.h"

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

/* Number of plugins in plugin index. */
uint32_t plugin_count;

/* Plugin context index. */
static struct plugin_ctx **plugins;

int32_t supported_file(const char *file_extension, unsigned int *plugin_offset)
{

	return (0);
}

static int32_t count_plugins(uint32_t *count)
{
	int32_t rtrn = 0;
	struct dirent *entry = NULL;
	DIR *dir auto_close_dir = NULL;
	char *file_extension auto_clean = NULL;
    const char *dir_path = "src/plugins";

    /* Open the plugin directory. */
    dir = opendir(dir_path);
    if(dir == NULL)
    {
    	output(ERROR, "Can't open dir: %s\n", strerror(errno));
    	return -1;
    }

     /* Walk the directory. */
    while((entry = readdir(dir)) != NULL)
    {
    	/* Skip hidden files. */
        if(entry->d_name[0] == '.')
            continue;

        /* Get file extension. */
        rtrn = get_extension(entry->d_name, &file_extension);
        if(rtrn < 0)
        {
        	output(ERROR, "Can't get file extension\n");
        	return (-1);
        }

        /* Increment the plugin count if the file is a shared object. */
        if(strncmp(file_extension, ".so", 3) == 0)
        	(*count)++;
    }

    return 0;
}

 int32_t features_supported(unsigned int offset, unsigned int *number_of_features)
 {
 	return 0;
 }

 int32_t feature_constraints(unsigned int offset, struct feature_constraints *constraints)
 {

 	return 0;
 }

static int32_t load_all_plugins(void)
{
    /* Tell user our course of action. */
    if(plugin_count == 0)
    {
    	output(STD, "No plugins to load\n");
    	return 0;
    }
    
    output(STD, "Loading all plugins\n");

    uint32_t i = 0;
    int32_t rtrn = 0;
	struct dirent *entry = NULL;
	DIR *dir auto_close_dir = NULL;
	char *file_extension auto_clean = NULL;
    const char *dir_path = "src/plugins";

    /* Open the plugin directory. */
    dir = opendir(dir_path);
    if(dir == NULL)
    {
    	output(ERROR, "Can't open dir: %s\n", strerror(errno));
    	return -1;
    }

    /* Walk the directory. */
    while((entry = readdir(dir)) != NULL)
    {
    	/* Skip hidden files. */
        if(entry->d_name[0] == '.')
            continue;

        /* Get file extension. */
        rtrn = get_extension(entry->d_name, &file_extension);
        if(rtrn < 0)
        {
        	output(ERROR, "Can't get file extension\n");
        	return -1;
        }

        /* It's a shared library so it's probally a plugin. */
        if(strncmp(file_extension, ".so", 3) == 0)
        {
        	char *plugin_path auto_clean = NULL;
        	char *current_dir auto_clean = NULL;

        	current_dir = malloc(1024);
        	if(current_dir == NULL)
        	{
        		output(ERROR, "Can't malloc current_dir buf: %s\n", strerror(errno));
        		return -1;
        	}

        	/* Get the current directory path. */
        	current_dir = getcwd(current_dir, 1023);
        	if(current_dir == NULL)
        	{
                output(ERROR, "Can't get current directory: %s\n");
                return -1;
        	}

            /* Create plugin path string. */
        	rtrn = asprintf(&plugin_path, "%s/src/plugins/%s", current_dir, entry->d_name);
        	if(rtrn < 0)
        	{
        		output(ERROR, "Can't create plugin path string: %s\n", strerror(errno));
        		return -1;
        	}

        	/* Set the plugin path in the plugin context.. */
        	rtrn = asprintf((char **)&plugins[i]->plugin_path, "%s", plugin_path);
            if(rtrn < 0)
        	{
        		output(ERROR, "Can't set plugin path string: %s\n", strerror(errno));
        		return -1;
        	}

        	/* Set the plugin name in the plugin context.. */
        	rtrn = asprintf((char **)&plugins[i]->plugin_name, "%s", basename(plugin_path));
            if(rtrn < 0)
        	{
        		output(ERROR, "Can't set plugin name string: %s\n", strerror(errno));
        		return -1;
        	}

        	/* Actually load the plugin. */
        	plugins[i]->plugin_handle = dlopen(plugin_path, RTLD_NOW);
        	if(plugins[i]->plugin_handle == NULL)
        	{
        		output(ERROR, "Can't load plugin: %s beacause: %s\n", basename(plugin_path), strerror(errno));
        		return -1;
        	}

        	i++;
        }
    }

	return 0;
}

int32_t setup_plugin_module(void)
{
    int32_t rtrn = 0;

    rtrn = count_plugins(&plugin_count);
    if(rtrn < 0)
    {
        output(ERROR, "Can't count plugins\n");
        return -1;
    }
    
    /* Load all the plugins in the nextgen/src/plugins/ directory.*/
    rtrn = load_all_plugins();
    if(rtrn < 0)
    {
        output(ERROR, "Can't load plugins\n");
        return -1;
    }

    return (0);
}

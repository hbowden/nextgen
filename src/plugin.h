

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

#ifndef PLUGIN_H
#define PLUGIN_H

#ifdef CPLUS_PLUS

extern "C" {

#endif

#include "private.h"

/* This struct is used as a plugin handle. */
struct plugin_ctx
{
    const char *plugin_name;
    const char *plugin_path;
    void *plugin_handle;

};

/* This struct abstracts away specific file formats features as a generic struct. */
struct feature_constraints
{
    unsigned int feature_number;

};

private extern int load_all_plugins(void);

private extern int count_plugins(unsigned int *count);

private extern int supported_file(const char *file_extension, unsigned int *plugin_offset);

private extern int features_supported(unsigned int offset, unsigned int *number_of_features);

private extern int feature_constraints(unsigned int offset, struct feature_constraints *constraints);

#ifdef CPLUS_PLUS

}

#endif

/* End of header file. */
#endif

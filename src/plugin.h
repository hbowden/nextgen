

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

#ifndef PLUGIN_H
#define PLUGIN_H

#include <stdint.h>

#ifdef CPLUS_PLUS

extern "C" {

#endif

/* Number of plugins in plugin index. */
extern uint32_t plugin_count;

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
    uint32_t feature_number;

};

extern int32_t setup_plugin_module(void);

extern int32_t supported_file(const char *file_extension, unsigned int *plugin_offset);

extern int32_t features_supported(unsigned int offset, unsigned int *number_of_features);

extern int32_t feature_constraints(unsigned int offset, struct feature_constraints *constraints);

#ifdef CPLUS_PLUS

}

#endif

/* End of header file. */
#endif

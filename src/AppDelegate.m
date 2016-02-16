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

#import "AppDelegate.h"

#include "nextgen.h"
#include "runtime/runtime.h"
#include "memory/memory.h"
#include "io/io.h"

@implementation MyApplicationDelegate : NSObject
- (id)init {
    if ((self = [super init]))
    {
        int32_t rtrn = 0;

        /* Setup the fuzzer running enviroment. */
        rtrn = setup_runtime();
        if(rtrn < 0)
        {
            output(ERROR, "Can't setup runtime enviroment.\n");
            clean_shared_mapping();
            //return;
        }
    }
    return self;
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification {
    [window makeKeyAndOrderFront:self];
}

- (void)dealloc {
    [window release];
    [super dealloc];
}

@end

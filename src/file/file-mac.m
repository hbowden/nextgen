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

#include "file-mac.h"
#include "io/io.h"
#include "memory/memory.h"
#include "objc/objc-utils.h"
#include "utils/utils.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

static id self_ref;

@interface test_case : NSObject
@end

@implementation test_case : NSObject

- (void)startup_handler:(NSNotification *)not
{
    NSLog(@"Notification arrived");
}
@end

int32_t run_test_case(char *exec_path, char *file_path, char *file_extension)
{
    NSNotificationCenter *notification;
    NSString *target_path = cstring_to_nsstring(exec_path);
    NSString *test_case_path = cstring_to_nsstring(file_path);

    notification = [[NSWorkspace sharedWorkspace] notificationCenter];
    test_case *t = [[test_case alloc] init];

    self_ref = t;

    [notification addObserver:self_ref
                     selector:@selector(startup_handler:)
                         name:NSWorkspaceDidLaunchApplicationNotification
                       object:nil];

    /* Launch the target application with the test case given to us. */
    [[NSWorkspace sharedWorkspace] openFile:test_case_path
                            withApplication:target_path];

    return (0);
}

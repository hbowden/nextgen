// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>
#include "utils.h"
#include "io/io.h"

void set_process_name(char *name, struct output_writter *output)
{

  CFStringRef process_name;

  process_name = CFStringCreateWithCString(kCFAllocatorDefault,
                                           name,
                                           kCFStringEncodingMacRoman);

  // Warning: here be dragons! This is SPI reverse-engineered from WebKit's
  // plugin host, and could break at any time (although realistically it's only
  // likely to break in a new major release).
  // When 10.7 is available, check that this still works, and update this
  // comment for 10.8.

  // Private CFType used in these LaunchServices calls.
  typedef CFTypeRef PrivateLSASN;
  typedef PrivateLSASN (*LSGetCurrentApplicationASNType)();
  typedef OSStatus (*LSSetApplicationInformationItemType)(int, PrivateLSASN,
                                                          CFStringRef,
                                                          CFStringRef,
                                                          CFDictionaryRef*);

  static LSGetCurrentApplicationASNType ls_get_current_application_asn_func =
      NULL;
  static LSSetApplicationInformationItemType
      ls_set_application_information_item_func = NULL;
  static CFStringRef ls_display_name_key = NULL;

  static bool did_symbol_lookup = false;
  if (!did_symbol_lookup) {
    did_symbol_lookup = true;

    CFBundleRef launch_services_bundle =
        CFBundleGetBundleWithIdentifier(CFSTR("com.apple.LaunchServices"));
    if(!launch_services_bundle)
    {
      output->write(ERROR, "Failed to look up LaunchServices bundle\n");
      return;
    }

    ls_get_current_application_asn_func = CFBundleGetFunctionPointerForName(launch_services_bundle,
                                                                            CFSTR("_LSGetCurrentApplicationASN"));
    if(!ls_get_current_application_asn_func)
      output->write(ERROR, "Could not find _LSGetCurrentApplicationASN\n");

    ls_set_application_information_item_func =
            CFBundleGetFunctionPointerForName(
                launch_services_bundle,
                CFSTR("_LSSetApplicationInformationItem"));
    if (!ls_set_application_information_item_func)
      output->write(ERROR, "Could not find _LSSetApplicationInformationItem\n");

    CFStringRef* key_pointer =
        CFBundleGetDataPointerForName(launch_services_bundle,
                                      CFSTR("_kLSDisplayNameKey"));
    ls_display_name_key = key_pointer ? *key_pointer : NULL;
    if (!ls_display_name_key)
      output->write(ERROR, "Could not find _kLSDisplayNameKey\n");

    // Internally, this call relies on the Mach ports that are started up by the
    // Carbon Process Manager.  In debug builds this usually happens due to how
    // the logging layers are started up; but in release, it isn't started in as
    // much of a defined order.  So if the symbols had to be loaded, go ahead
    // and force a call to make sure the manager has been initialized and hence
    // the ports are opened.
    ProcessSerialNumber psn;
    GetCurrentProcess(&psn);
  }
  if (!ls_get_current_application_asn_func ||
      !ls_set_application_information_item_func ||
      !ls_display_name_key) {
    return;
  }

  PrivateLSASN asn = ls_get_current_application_asn_func();
  // Constant used by WebKit; what exactly it means is unknown.
  const int magic_session_constant = -2;

  ls_set_application_information_item_func(magic_session_constant, asn,
                                           ls_display_name_key,
                                           process_name,
                                           NULL /* optional out param */);
}

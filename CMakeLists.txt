cmake_minimum_required (VERSION 3.0)

# Project name and programming language.
project (Nextgen C)

# Turn on testing.
enable_testing()

# Save concurrencykit name as a variable.
SET(CK "ck-0.5.2")
SET(UNITY "Unity")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-documentation -fms-extensions -Wno-microsoft-anon-tag")

# Set the cmake module path to deps/cmake_modules.
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/deps/cmake_modules" ${CMAKE_MODULE_PATH})

# Look for the compiler sanitizers cmake module.
find_package(Sanitizers)

# Include the lcov cmake module so we can generate code coverage reports.
INCLUDE(CodeCoverage)

# Turn on coverage for debug builds.

# Create a coverage target.
SETUP_TARGET_FOR_COVERAGE(coverage ctest coverage)

# The warnings to ignore for src/syscall/generate.* .
set(GENERATE_IGNORE_WARNINGS "-Wno-deprecated-declarations -Wno-cast-align")

# Ignore deprecated ptrace symbols because we need to test ptrace.
set_source_files_properties(src/syscall/generate.c PROPERTIES COMPILE_FLAGS ${GENERATE_IGNORE_WARNINGS})
set_source_files_properties(src/syscall/generate-macos.c PROPERTIES COMPILE_FLAGS ${GENERATE_IGNORE_WARNINGS})

SET(SET_TEST_IGNORE_FLAGS "-Wno-deprecated-declarations -Wno-unused-function")
SET(SET_FILE_IGNORE_FLAGS "-Wno-deprecated-declarations -Wno-cast-qual")
SET(SET_RUNTIME_IGNORE_FLAGS "-Wno-deprecated-declarations -Wno-incompatible-pointer-types-discards-qualifiers")

set_source_files_properties(src/io/io.c PROPERTIES COMPILE_FLAGS -Wno-format-nonliteral)
set_source_files_properties(src/plugins/plugin.c PROPERTIES COMPILE_FLAGS -Wno-cast-qual)
set_source_files_properties(src/syscall/set_test.c PROPERTIES COMPILE_FLAGS ${SET_TEST_IGNORE_FLAGS})
set_source_files_properties(src/file/file.c PROPERTIES COMPILE_FLAGS ${SET_FILE_IGNORE_FLAGS})
set_source_files_properties(src/probe/probe-freebsd.c PROPERTIES COMPILE_FLAGS -Wno-incompatible-pointer-types-discards-qualifiers)
set_source_files_properties(src/runtime/nextgen.c PROPERTIES COMPILE_FLAGS ${SET_RUNTIME_IGNORE_FLAGS})
set_source_files_properties(src/resource/resource.c PROPERTIES COMPILE_FLAGS -Wno-incompatible-pointer-types-discards-qualifiers)
set_source_files_properties(src/concurrent/concurrent-freebsd.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties(src/main.m PROPERTIES COMPILE_FLAGS -Wno-incompatible-pointer-types-discards-qualifiers)

# Check the compiler and set compiler specific options.
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  # using Clang
  MESSAGE( WARNING "Using clang")
  SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything -std=c11")
  SET( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Weverything -std=c11")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  add_definitions("-Wextra -std=gnu11")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()

# Set include directories so we can find headers.
include_directories(src)
include_directories(SYSTEM deps/sqlite)
include_directories(SYSTEM deps/libressl-2.5.0/include)
include_directories(SYSTEM deps/${CK}/include)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/deps/capstone-3.0.4/include SYSTEM)

link_directories(${CMAKE_SOURCE_DIR}/deps/libressl-2.5.0/crypto/.libs)
link_directories(${CMAKE_SOURCE_DIR}/deps/sqlite)
link_directories(${CMAKE_SOURCE_DIR}/deps/${CK}/src)
link_directories(${CMAKE_SOURCE_DIR}/deps/capstone-3.0.4)

# Build and link libraries that don't need os specific build instructions.
add_library(nxio SHARED src/io/io.c)
add_library(nxmemory SHARED src/memory/memory.c)
add_library(nxconcurrent SHARED src/concurrent/concurrent.c src/concurrent/epoch.c)
target_link_libraries(nxmemory nxio)

# Check the operating system and set flags that are os specific.
if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    # We need this to conditionally compile freebsd only sources.
    add_definitions(-DFREEBSD)

    SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything -std=c11 -Wno-covered-switch-default")
    SET( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Weverything -std=c11 -Wno-covered-switch-default")

    add_library(nxutils SHARED src/utils/utils.c src/utils/reallocarray.c)

    add_library(nxcrypto SHARED src/crypto/crypto.c)

    include_directories(SYSTEM /usr/src/cddl/compat/opensolaris/include)
    include_directories(SYSTEM /usr/src/cddl/contrib/opensolaris/lib/libdtrace/common)
    include_directories(SYSTEM /usr/src/sys/cddl/compat/opensolaris)
    include_directories(SYSTEM /usr/src/sys/cddl/contrib/opensolaris/uts/common)

    add_library(nxprobe SHARED src/probe/probe.c src/probe/probe-freebsd.c)

    target_link_libraries(nxprobe dtrace)

    add_library(nxnetwork SHARED src/network/network.c src/network/network-bsd.c)

    add_library(nxplugin SHARED src/plugins/plugin.c)
    add_library(nxmutate SHARED src/mutate/mutate.c)
    add_library(nxresource SHARED src/resource/resource.c)

    add_library(nxlog SHARED src/log/log.c src/log/log-freebsd.c)
    target_link_libraries(nxlog ${CMAKE_SOURCE_DIR}/deps/sqlite/sqlite3.so)

    AUX_SOURCE_DIRECTORY(src/syscall/freebsd ENTRY_SOURCES)
    add_library(nxsyscall SHARED src/syscall/syscall.c src/syscall/syscall-freebsd.c src/syscall/signals.c src/syscall/set_test.c src/syscall/generate.c src/syscall/arg_types.c ${ENTRY_SOURCES})
    include_directories(src/syscall/freebsd)

    add_library(nxgenetic SHARED src/genetic/genetic.c)

    add_library(nxfile SHARED src/file/file.c src/file/file-freebsd.c)

    add_library(nxdisas SHARED src/disas/disas.c src/disas/disas-freebsd.c)
    target_link_libraries(nxdisas ${CMAKE_SOURCE_DIR}/deps/capstone-3.0.4/libcapstone.so)

    add_library(nxruntime SHARED src/runtime/runtime.c src/runtime/fuzzer-syscall.c src/runtime/nextgen.c)

    add_executable(nextgen src/main.c)
    target_link_libraries(nextgen dtrace)
    target_link_libraries(nextgen proc)
    target_link_libraries(nextgen ctf)
    target_link_libraries(nextgen elf)
    target_link_libraries(nextgen z)
    target_link_libraries(nextgen rtld_db)
    target_link_libraries(nextgen pthread)
    target_link_libraries(nextgen util)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    # We need this to conditionally compile macOS only sources.
    add_definitions(-DMAC_OS)

    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything -std=c11 -Wno-covered-switch-default -Wno-switch-enum")
    SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Weverything -std=c11 -Wno-covered-switch-default  -Wno-switch-enum")

    SET(UTILS_IGNORE_FLAGS "-Wno-reserved-id-macro -Wno-unused-macros")
    set_source_files_properties(src/utils/utils.c PROPERTIES COMPILE_FLAGS ${UTILS_IGNORE_FLAGS})

    # We currently need a deprecated carbon function to set a process name on OSX.
    set_source_files_properties(src/utils/set_process_name.c PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations -Wno-pedantic")

    # Set standard compiler flags for all files.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fstack-protector-all -fno-strict-aliasing -Werror -Wno-unknown-warning-option -Wno-used-but-marked-unused -Wno-padded")

    # Ignore some objective-c specific warnings.
    set(OBJC_IGNORE_FLAGS "${CMAKE_C_FLAGS} -Wno-direct-ivar-access -Wno-objc-interface-ivars -Wno-unused-parameter")
    set_source_files_properties(src/objc/AppDelegate.m PROPERTIES COMPILE_FLAGS ${OBJC_IGNORE_FLAGS})
    set_source_files_properties(src/objc/AppDelegate.h PROPERTIES COMPILE_FLAGS ${OBJC_IGNORE_FLAGS})
    set_source_files_properties(src/objc/objc-utils.m PROPERTIES COMPILE_FLAGS ${OBJC_IGNORE_FLAGS})

    FIND_LIBRARY(CARBON Carbon)
    FIND_LIBRARY(CORE_FOUNDATION CoreFoundation)
    add_library(nxutils SHARED src/utils/utils.c src/utils/set_process_name.c src/utils/reallocarray.c)
    target_link_libraries(nxutils ${CORE_FOUNDATION})
    target_link_libraries(nxutils ${CARBON})

    add_library(nxcrypto SHARED src/crypto/crypto.c)

    add_library(nxprobe SHARED src/probe/probe.c src/probe/probe-mac.c)

    target_link_libraries(nxprobe dtrace)

    add_library(nxnetwork SHARED src/network/network.c src/network/network-bsd.c)

    add_library(nxplugin SHARED src/plugins/plugin.c)
    add_library(nxmutate SHARED src/mutate/mutate.c)
    add_library(nxresource SHARED src/resource/resource.c)

    add_library(nxlog SHARED src/log/log.c src/log/log-mac.c)
    target_link_libraries(nxlog ${CMAKE_SOURCE_DIR}/deps/sqlite/libsqlite3.0.dylib)

    AUX_SOURCE_DIRECTORY(src/syscall/mac ENTRY_SOURCES)

    add_library(nxsyscall SHARED src/syscall/syscall.c src/syscall/syscall-mac.c src/syscall/signals.c src/syscall/set_test.c src/syscall/generate.c src/syscall/generate-macos.c src/syscall/arg_types.c ${ENTRY_SOURCES})

    include_directories(src/syscall/mac)

    add_library(nxgenetic SHARED src/genetic/genetic.c)

    FIND_LIBRARY(APP_KIT AppKit)
    FIND_LIBRARY(FOUNDATION Foundation)
    add_library(nxobjcutils SHARED src/objc/objc-utils.m src/objc/AppDelegate.m)
    target_link_libraries(nxobjcutils ${APP_KIT})
    target_link_libraries(nxobjcutils ${FOUNDATION})

    add_library(nxfile SHARED src/file/file.c src/file/file-mac.m)
    target_link_libraries(nxfile objc)
    target_link_libraries(nxfile nxobjcutils)

    add_library(nxdisas SHARED src/disas/disas.c src/disas/disas-mac.c)
    target_link_libraries(nxfile objc)
    target_link_libraries(nxdisas ${CMAKE_SOURCE_DIR}/deps/capstone-3.0.4/libcapstone.dylib)

    add_library(nxruntime SHARED src/runtime/runtime.c src/runtime/fuzzer-syscall.c src/runtime/nextgen.c)

    add_executable(nextgen src/main.m)
    add_sanitizers(nextgen)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DLINUX)
    add_definitions(-DCOMMON)

    add_library(nxutils SHARED src/utils/utils.c src/utils/reallocarray.c)
    add_library(nxcrypto SHARED src/crypto/crypto.c)
    add_library(nxprobe SHARED src/probe/probe.c src/probe/probe-linux.c)
    add_library(nxnetwork SHARED src/network/network.c src/network/network-linux.c)
    add_library(nxplugin SHARED src/plugins/plugin.c)
    target_link_libraries(nxplugin dl)
    add_library(nxmutate SHARED src/mutate/mutate.c)
    add_library(nxresource SHARED src/resource/resource.c)

    # The freebsd log file works on linux so we use it.
    add_library(nxlog SHARED src/log/log.c src/log/log-freebsd.c)
    target_link_libraries(nxlog sqlite3)

    AUX_SOURCE_DIRECTORY(src/syscall/linux ENTRY_SOURCES)
    add_library(nxsyscall SHARED src/syscall/syscall.c src/syscall/syscall-linux.c src/syscall/signals.c src/syscall/set_test.c src/syscall/generate.c src/syscall/arg_types.c ${ENTRY_SOURCES})
    include_directories(src/syscall/linux)

    add_library(nxgenetic SHARED src/genetic/genetic.c)

    add_library(nxfile SHARED src/file/file.c src/file/file-linux.c)

    add_library(nxdisas SHARED src/disas/disas.c src/disas/disas-linux.c)
    target_link_libraries(nxdisas ${CMAKE_SOURCE_DIR}/deps/capstone-3.0.4/libcapstone.so)

    add_library(nxruntime SHARED src/runtime/runtime.c src/runtime/fuzzer-syscall.c src/runtime/nextgen.c)
    target_link_libraries(nxruntime pthread)

    add_executable(nextgen src/main.c)
    target_link_libraries(nextgen pthread)
    target_link_libraries(nextgen dl)

endif()

# Do os agnostic linking.
target_link_libraries(nxutils nxcrypto)
target_link_libraries(nxutils nxio)
target_link_libraries(nxutils nxmemory)
target_link_libraries(nxconcurrent nxio)
target_link_libraries(nxconcurrent nxmemory)
target_link_libraries(nxconcurrent nxutils)
target_link_libraries(nxconcurrent ${CMAKE_SOURCE_DIR}/deps/${CK}/src/libck.so)
target_link_libraries(nxcrypto crypto)
target_link_libraries(nxcrypto nxio)
target_link_libraries(nxcrypto nxmemory)
target_link_libraries(nxprobe nxio)
target_link_libraries(nxprobe nxutils)
target_link_libraries(nxnetwork nxcrypto)
target_link_libraries(nxnetwork nxio)
target_link_libraries(nxnetwork nxmemory)
target_link_libraries(nxplugin nxutils)
target_link_libraries(nxplugin nxio)
target_link_libraries(nxplugin nxmemory)
target_link_libraries(nxmutate nxcrypto)
target_link_libraries(nxmutate nxutils)
target_link_libraries(nxmutate nxplugin)
target_link_libraries(nxmutate nxio)
target_link_libraries(nxmutate nxmemory)
target_link_libraries(nxresource nxcrypto)
target_link_libraries(nxresource nxutils)
target_link_libraries(nxresource nxnetwork)
target_link_libraries(nxresource nxio)
target_link_libraries(nxresource nxmemory)
target_link_libraries(nxlog nxio)
target_link_libraries(nxlog nxmemory)
target_link_libraries(nxsyscall ${CMAKE_SOURCE_DIR}/deps/${CK}/src/libck.so)
target_link_libraries(nxsyscall nxio)
target_link_libraries(nxsyscall nxmemory)
target_link_libraries(nxsyscall nxconcurrent)
target_link_libraries(nxsyscall nxprobe)
target_link_libraries(nxsyscall nxresource)
target_link_libraries(nxsyscall nxutils)
target_link_libraries(nxsyscall nxlog)
target_link_libraries(nxsyscall nxmutate)
target_link_libraries(nxsyscall nxnetwork)
target_link_libraries(nxsyscall nxcrypto)
target_link_libraries(nxgenetic nxsyscall)
target_link_libraries(nxgenetic nxio)
target_link_libraries(nxgenetic nxmemory)
target_link_libraries(nxfile nxsyscall)
target_link_libraries(nxfile nxio)
target_link_libraries(nxfile nxmemory)
target_link_libraries(nxdisas nxio)
target_link_libraries(nxdisas nxfile)
target_link_libraries(nxdisas nxmemory)
target_link_libraries(nxruntime ${CMAKE_SOURCE_DIR}/deps/${CK}/src/libck.so)
target_link_libraries(nxruntime nxio)
target_link_libraries(nxruntime nxmemory)
target_link_libraries(nxruntime nxconcurrent)
target_link_libraries(nxruntime nxprobe)
target_link_libraries(nxruntime nxresource)
target_link_libraries(nxruntime nxutils)
target_link_libraries(nxruntime nxlog)
target_link_libraries(nxruntime nxmutate)
target_link_libraries(nxruntime nxnetwork)
target_link_libraries(nxruntime nxcrypto)
target_link_libraries(nxruntime nxdisas)
target_link_libraries(nxruntime nxsyscall)
target_link_libraries(nxruntime nxfile)
target_link_libraries(nxruntime nxprobe)
target_link_libraries(nxruntime nxgenetic)
target_link_libraries(nextgen nxutils)
target_link_libraries(nextgen nxruntime)
target_link_libraries(nextgen nxmemory)
target_link_libraries(nextgen nxio)

install(TARGETS nextgen nxio nxmemory nxconcurrent nxcrypto nxutils nxprobe nxnetwork nxplugin nxmutate nxresource nxlog nxsyscall nxgenetic nxfile nxdisas nxruntime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

install(CODE "execute_process(COMMAND \"../install.sh\")")
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

include_directories(deps/${UNITY}/src)

add_executable(concurrent-unit-test EXCLUDE_FROM_ALL tests/concurrent/unit/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(concurrent-unit-test nxconcurrent)
target_link_libraries(concurrent-unit-test nxmemory)
target_link_libraries(concurrent-unit-test ${CMAKE_SOURCE_DIR}/deps/${CK}/src/libck.so)
target_link_libraries(concurrent-unit-test pthread)

add_executable(memory-unit-test EXCLUDE_FROM_ALL tests/memory/unit/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(memory-unit-test nxconcurrent)
target_link_libraries(memory-unit-test nxmemory)
target_link_libraries(memory-unit-test ${CMAKE_SOURCE_DIR}/deps/${CK}/src/libck.so)

add_executable(memory-intergration-test EXCLUDE_FROM_ALL tests/memory/unit/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(memory-intergration-test nxconcurrent)
target_link_libraries(memory-intergration-test nxmemory)
target_link_libraries(memory-intergration-test ${CMAKE_SOURCE_DIR}/deps/${CK}/src/libck.so)

add_executable(crypto-unit-test EXCLUDE_FROM_ALL tests/crypto/unit/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(crypto-unit-test nxconcurrent)
target_link_libraries(crypto-unit-test nxmemory)
target_link_libraries(crypto-unit-test nxcrypto)
target_link_libraries(crypto-unit-test crypto)
target_link_libraries(crypto-unit-test ${CMAKE_SOURCE_DIR}/deps/${CK}/src/libck.so)

add_executable(utils-unit-test EXCLUDE_FROM_ALL tests/utils/unit/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(utils-unit-test nxconcurrent)
target_link_libraries(utils-unit-test nxmemory)
target_link_libraries(utils-unit-test nxutils)
target_link_libraries(utils-unit-test nxcrypto)
target_link_libraries(utils-unit-test crypto)
target_link_libraries(utils-unit-test ${CMAKE_SOURCE_DIR}/deps/${CK}/src/libck.so)

add_executable(resource-integration-test EXCLUDE_FROM_ALL tests/resource/integration/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(resource-integration-test nxnetwork)
target_link_libraries(resource-integration-test nxconcurrent)
target_link_libraries(resource-integration-test nxmemory)
target_link_libraries(resource-integration-test nxutils)
target_link_libraries(resource-integration-test nxcrypto)
target_link_libraries(resource-integration-test crypto)

add_executable(runtime-integration-test EXCLUDE_FROM_ALL tests/runtime/integration/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(runtime-integration-test nxruntime)

add_sanitizers(runtime-integration-test)
add_sanitizers(utils-unit-test)
add_sanitizers(memory-unit-test)
add_sanitizers(memory-intergration-test)
add_sanitizers(crypto-unit-test)
add_sanitizers(concurrent-unit-test)
add_sanitizers(resource-integration-test)

add_test(resource-integration-test resource-integration-test)
add_test(memory-unit-test memory-unit-test)
add_test(memory-intergration-test memory-intergration-test)
add_test(crypto-unit-test crypto-unit-test)
add_test(concurrent-unit-test concurrent-unit-test)

add_dependencies(check resource-integration-test)
add_dependencies(check memory-intergration-test)
add_dependencies(check memory-unit-test)
add_dependencies(check crypto-unit-test)
add_dependencies(check concurrent-unit-test)

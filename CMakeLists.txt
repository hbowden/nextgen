cmake_minimum_required (VERSION 2.6)
project (Nextgen)
set(CMAKE_BUILD_TYPE Release)

# Check the compiler and set compiler specific options.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # using Clang
  add_definitions("-Weverything -std=c99")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  add_definitions("-Wextra -std=gnu99")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()

add_definitions("-Wall -fstack-protector-all -fno-strict-aliasing -g -Werror -Wno-unknown-warning-option -Wno-pedantic -Wno-deprecated-declarations -Wno-used-but-marked-unused -Wno-padded")

set_source_files_properties(src/io/io.c PROPERTIES COMPILE_FLAGS -Wno-format-nonliteral)

set_source_files_properties(src/plugins/plugin.c PROPERTIES COMPILE_FLAGS -Wno-cast-qual)

set_source_files_properties(src/syscall/set_test.c PROPERTIES COMPILE_FLAGS -Wno-unused-function)

set_source_files_properties(src/syscall/generate.c PROPERTIES COMPILE_FLAGS -Wno-cast-align)

set_source_files_properties(src/file/file.c PROPERTIES COMPILE_FLAGS -Wno-cast-qual)

set_source_files_properties(src/probe/probe-freebsd.c PROPERTIES COMPILE_FLAGS -Wno-incompatible-pointer-types-discards-qualifiers)

set_source_files_properties(src/runtime/runtime.c PROPERTIES COMPILE_FLAGS -Wno-incompatible-pointer-types-discards-qualifiers)

set_source_files_properties(src/resource/resource.c PROPERTIES COMPILE_FLAGS -Wno-incompatible-pointer-types-discards-qualifiers)

set_source_files_properties(src/concurrent/concurrent-freebsd.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)

set_source_files_properties(src/main.m PROPERTIES COMPILE_FLAGS -Wno-incompatible-pointer-types-discards-qualifiers)

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    add_definitions(-DFREEBSD)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DMAC_OSX)
    add_definitions(-Wno-direct-ivar-access)
    set_source_files_properties(src/objc/AppDelegate.m PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
    set_source_files_properties(src/utils/SetProcessName.c PROPERTIES COMPILE_FLAGS -Wno-deprecated-declarations)
    add_definitions(-Wno-objc-interface-ivars)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DLINUX)
    add_definitions(-DCOMMON)
endif() 

include_directories(src)
include_directories(SYSTEM deps/sqlite)
include_directories(SYSTEM deps/libressl-2.4.1/include)
include_directories(SYSTEM deps/ck-0.5.1/include SYSTEM)

add_library(nxio SHARED src/io/io.c)
add_library(nxmemory SHARED src/memory/memory.c)
target_link_libraries(nxmemory nxio)

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    add_library(nxconcurrent SHARED src/concurrent/concurrent.c src/concurrent/concurrent-freebsd.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_library(nxconcurrent SHARED src/concurrent/concurrent.c src/concurrent/concurrent-mac.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_library(nxconcurrent SHARED src/concurrent/concurrent.c src/concurrent/concurrent-common.c)
endif()

target_link_libraries(nxconcurrent nxio)

link_directories(${CMAKE_SOURCE_DIR}/deps/libressl-2.4.1/crypto/.libs)
add_library(nxcrypto SHARED src/crypto/crypto.c)
target_link_libraries(nxcrypto crypto)
target_link_libraries(nxcrypto nxio)
target_link_libraries(nxcrypto nxmemory)

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
   add_library(nxutils SHARED src/utils/utils.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    FIND_LIBRARY(CARBON Carbon)
    FIND_LIBRARY(CORE_FOUNDATION CoreFoundation)
    add_library(nxutils SHARED src/utils/utils.c src/utils/SetProcessName.c)
    target_link_libraries(nxutils ${CORE_FOUNDATION})
    target_link_libraries(nxutils ${CARBON})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_library(nxutils SHARED src/utils/utils.c)
endif()

target_link_libraries(nxutils nxcrypto)
target_link_libraries(nxutils nxio)
target_link_libraries(nxutils nxmemory)

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    include_directories(SYSTEM /usr/src/cddl/compat/opensolaris/include)
    include_directories(SYSTEM /usr/src/cddl/contrib/opensolaris/lib/libdtrace/common)
    include_directories(SYSTEM /usr/src/sys/cddl/compat/opensolaris)
    include_directories(SYSTEM /usr/src/sys/cddl/contrib/opensolaris/uts/common)
    add_library(nxprobe SHARED src/probe/probe.c src/probe/probe-freebsd.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_library(nxprobe SHARED src/probe/probe.c src/probe/probe-mac.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_library(nxprobe SHARED src/probe/probe.c src/probe/probe-linux.c)
endif()

target_link_libraries(nxprobe nxio)
target_link_libraries(nxprobe nxutils)

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    target_link_libraries(nxprobe dtrace)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   target_link_libraries(nxprobe dtrace)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
 
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    add_library(nxnetwork SHARED src/network/network.c  src/network/network-bsd.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   add_library(nxnetwork SHARED src/network/network.c  src/network/network-bsd.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_library(nxnetwork SHARED src/network/network.c  src/network/network-linux.c)
endif()

target_link_libraries(nxnetwork nxcrypto)
target_link_libraries(nxnetwork nxio)
target_link_libraries(nxnetwork nxmemory)

add_library(nxplugin SHARED src/plugins/plugin.c)
target_link_libraries(nxplugin nxutils)
target_link_libraries(nxplugin nxio)
target_link_libraries(nxplugin nxmemory)

add_library(nxmutate SHARED src/mutate/mutate.c)
target_link_libraries(nxmutate nxcrypto)
target_link_libraries(nxmutate nxutils)
target_link_libraries(nxmutate nxplugin)
target_link_libraries(nxmutate nxio)
target_link_libraries(nxmutate nxmemory)

add_library(nxresource SHARED src/resource/resource.c)
target_link_libraries(nxresource nxcrypto)
target_link_libraries(nxresource nxutils)
target_link_libraries(nxresource nxnetwork)
target_link_libraries(nxresource nxio)
target_link_libraries(nxresource nxmemory)

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
   add_library(nxlog SHARED src/log/log.c src/log/log-freebsd.c)
   target_link_libraries(nxlog ${CMAKE_SOURCE_DIR}/deps/sqlite/sqlite3.so)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   add_library(nxlog SHARED src/log/log.c src/log/log-mac.c)
   target_link_libraries(nxlog ${CMAKE_SOURCE_DIR}/deps/sqlite/libsqlite3.0.dylib)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_library(nxlog SHARED src/log/log.c src/log/log-freebsd.c)
   target_link_libraries(nxlog ${CMAKE_SOURCE_DIR}/deps/sqlite/libsqlite3.so)
endif()

link_directories(${CMAKE_SOURCE_DIR}/deps/sqlite)

target_link_libraries(nxlog nxio)
target_link_libraries(nxlog nxmemory)

link_directories(${CMAKE_SOURCE_DIR}/deps/ck-0.5.1/src)

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    AUX_SOURCE_DIRECTORY(src/syscall/freebsd ENTRY_SOURCES) 
    add_library(nxsyscall SHARED src/syscall/syscall.c src/syscall/syscall-freebsd.c src/syscall/signals.c src/syscall/set_test.c src/syscall/generate.c src/syscall/arg_types.c ${ENTRY_SOURCES})
    include_directories(src/syscall/freebsd)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    AUX_SOURCE_DIRECTORY(src/syscall/mac ENTRY_SOURCES) 
    add_library(nxsyscall SHARED src/syscall/syscall.c src/syscall/syscall-mac.c src/syscall/signals.c src/syscall/set_test.c src/syscall/generate.c src/syscall/arg_types.c ${ENTRY_SOURCES})
    include_directories(src/syscall/mac)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    AUX_SOURCE_DIRECTORY(src/syscall/linux ENTRY_SOURCES) 
    add_library(nxsyscall SHARED src/syscall/syscall.c src/syscall/syscall-linux.c src/syscall/signals.c src/syscall/set_test.c src/syscall/generate.c src/syscall/arg_types.c ${ENTRY_SOURCES})
    include_directories(src/syscall/linux)
endif()

target_link_libraries(nxsyscall ${CMAKE_SOURCE_DIR}/deps/ck-0.5.1/src/libck.so)
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

add_library(nxgenetic SHARED src/genetic/genetic.c)
target_link_libraries(nxgenetic nxsyscall)
target_link_libraries(nxgenetic nxio)
target_link_libraries(nxgenetic nxmemory)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    FIND_LIBRARY(APP_KIT AppKit)
    FIND_LIBRARY(FOUNDATION Foundation)
    add_library(nxobjcutils SHARED src/objc/objc-utils.m src/objc/AppDelegate.m)
    target_link_libraries(nxobjcutils ${APP_KIT})
    target_link_libraries(nxobjcutils ${FOUNDATION})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    add_library(nxfile SHARED src/file/file.c src/file/file-freebsd.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_library(nxfile SHARED src/file/file.c src/file/file-mac.m)
    target_link_libraries(nxfile objc)
    target_link_libraries(nxfile nxobjcutils)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_library(nxfile SHARED src/file/file.c src/file/file-linux.c)
endif()

target_link_libraries(nxfile nxsyscall)
target_link_libraries(nxfile nxio)
target_link_libraries(nxfile nxmemory)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/deps/capstone-3.0.4/include SYSTEM)

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    add_library(nxdisas SHARED src/disas/disas.c src/disas/disas-freebsd.c)
    target_link_libraries(nxdisas ${CMAKE_SOURCE_DIR}/deps/capstone-3.0.4/libcapstone.so)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_library(nxdisas SHARED src/disas/disas.c src/disas/disas-mac.c)
    target_link_libraries(nxfile objc)
    target_link_libraries(nxdisas ${CMAKE_SOURCE_DIR}/deps/capstone-3.0.4/libcapstone.dylib)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_library(nxdisas SHARED src/disas/disas.c src/disas/disas-linux.c)
    target_link_libraries(nxdisas ${CMAKE_SOURCE_DIR}/deps/capstone-3.0.4/libcapstone.so)
endif()

link_directories(${CMAKE_SOURCE_DIR}/deps/capstone-3.0.4)

target_link_libraries(nxdisas nxio)
target_link_libraries(nxdisas nxfile)
target_link_libraries(nxdisas nxmemory)

add_library(nxruntime SHARED src/runtime/runtime.c src/runtime/nextgen.c)

target_link_libraries(nxruntime ${CMAKE_SOURCE_DIR}/deps/ck-0.5.1/src/libck.so)
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

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
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
    add_executable(nextgen src/main.m)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_executable(nextgen src/main.c)
    target_link_libraries(nextgen pthread)
    target_link_libraries(nextgen dl)
endif()

target_link_libraries(nextgen nxutils)
target_link_libraries(nextgen nxruntime)
target_link_libraries(nextgen nxmemory)
target_link_libraries(nextgen nxio)

install(TARGETS nextgen nxio nxmemory nxconcurrent nxcrypto nxutils nxprobe nxnetwork nxplugin nxmutate nxresource nxlog nxsyscall nxgenetic nxfile nxdisas nxruntime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

install(CODE "execute_process(COMMAND \"../install.sh\")")
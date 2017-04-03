cmake_minimum_required (VERSION 3.0)

project (Nextgen C)

SET(CK "ck-0.6.0")
SET(UNITY "Unity")
SET(LIBRESSL "libressl-2.5.1")
SET(CAPSTONE "capstone-3.0.5-rc2")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/deps/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(Sanitizers)

if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything -std=c11")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Weverything -std=c11")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  add_definitions("-Wextra -std=gnu11")
endif()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -g -Werror \
-fno-strict-aliasing \
-Wno-documentation \
-fms-extensions \
-Wno-microsoft-anon-tag \
-fno-omit-frame-pointer \
-fstack-protector-all \
-D_FORTIFY_SOURCE=2 \
-Wno-unknown-warning-option \
-fPIC \
-std=c11 \
-Wno-macro-redefined")

include_directories(src)
include_directories(SYSTEM deps/${SQLITE})
include_directories(SYSTEM deps/${LIBRESSL}/include)
include_directories(SYSTEM deps/${CK}/include)
include_directories(SYSTEM deps/${CAPSTONE}/include)
include_directories(src/syscall)

SET(SET_TEST_IGNORE_FLAGS "-Wno-deprecated-declarations -Wno-unused-function")
set_source_files_properties(src/syscall/set_test.c PROPERTIES COMPILE_FLAGS ${SET_TEST_IGNORE_FLAGS})

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  add_definitions(-DFREEBSD)
  SET(UTILS_OS_FILE "src/utils/utils-bsd.c")
  SET(NETWORK_OS_FILE "src/network/network-bsd.c")
  SET(MAIN "src/main.c")
  AUX_SOURCE_DIRECTORY(src/syscall/freebsd ENTRY_SOURCES)
  include_directories(src/syscall/freebsd)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DMAC_OS)
  SET(UTILS_OS_FILE "src/utils/utils-macos.c")
  SET(NETWORK_OS_FILE "src/network/network-bsd.c")
  SET(LOG_OS_FILES "src/log/log-mac.c")
  SET(MAIN "src/main.m")
  SET(SYSCALL_OS_FILES "src/syscall/syscall-macos.c")
  AUX_SOURCE_DIRECTORY(src/syscall/mac ENTRY_SOURCES)
  include_directories(src/syscall/mac)
  SET(OBJC_IGNORE_FLAGS "${CMAKE_C_FLAGS} -Wno-direct-ivar-access -Wno-objc-interface-ivars -Wno-unused-parameter")
  set_source_files_properties(src/objc/AppDelegate.m PROPERTIES COMPILE_FLAGS ${OBJC_IGNORE_FLAGS})
  set_source_files_properties(src/objc/AppDelegate.h PROPERTIES COMPILE_FLAGS ${OBJC_IGNORE_FLAGS})
  set_source_files_properties(src/objc/objc-utils.m PROPERTIES COMPILE_FLAGS ${OBJC_IGNORE_FLAGS})
  FIND_LIBRARY(CARBON Carbon)
  FIND_LIBRARY(CORE_FOUNDATION CoreFoundation)
  FIND_LIBRARY(APP_KIT AppKit)
  FIND_LIBRARY(FOUNDATION Foundation)
  SET(UTILS_LINK_FILES "${CARBON}")
  add_library(nxobjcutils SHARED src/objc/objc-utils.m src/objc/AppDelegate.m)
  target_link_libraries(nxobjcutils ${APP_KIT})
  target_link_libraries(nxobjcutils ${FOUNDATION})
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX)
  SET(UTILS_OS_FILE "src/utils/utils-linux.c")
  AUX_SOURCE_DIRECTORY(src/syscall/linux ENTRY_SOURCES)
  include_directories(src/syscall/linux)
endif()

add_library(nxdependinject SHARED src/depend-inject/depend-inject.c)
add_library(nxio SHARED src/io/io.c)
add_library(nxmemory SHARED src/memory/memory.c)
add_library(nxcrypto SHARED src/crypto/crypto.c src/crypto/hash.c src/crypto/random.c)
add_library(nxutils SHARED ${UTILS_OS_FILE} src/utils/utils.c src/utils/reallocarray.c)
add_library(nxconcurrent SHARED src/concurrent/concurrent.c src/concurrent/epoch.c)
add_library(nxmutate SHARED src/mutate/mutate.c)
add_library(nxnetwork SHARED src/network/network.c ${NETWORK_OS_FILE})
add_library(nxresource SHARED src/resource/resource.c)
add_library(nxlog SHARED src/log/log.c ${LOG_OS_FILES})
add_library(nxsyscall SHARED ${ENTRY_SOURCES} ${SYSCALL_OS_FILES} src/syscall/syscall.c src/syscall/generate.c src/syscall/set_test.c src/syscall/signals.c src/syscall/arg_types.c src/syscall/child.c)
add_library(nxgenetic SHARED src/genetic/genetic.c)
add_library(nxruntime SHARED src/runtime/runtime.c src/runtime/fuzzer.c src/runtime/fuzzer-syscall.c src/runtime/nextgen.c)

target_link_libraries(nxcrypto ${CMAKE_SOURCE_DIR}/deps/${LIBRESSL}/crypto/.libs/libcrypto.a)
target_link_libraries(nxnetwork nxcrypto)
target_link_libraries(nxutils ${UTILS_LINK_FILES})
target_link_libraries(nxresource nxnetwork nxmemory nxutils nxcrypto)
target_link_libraries(nxconcurrent ${CMAKE_SOURCE_DIR}/deps/${CK}/src/libck.so nxutils)
target_link_libraries(nxruntime nxio nxdependinject nxmemory nxsyscall)
target_link_libraries(nxsyscall nxconcurrent nxmutate)
target_link_libraries(nxruntime nxcrypto nxresource)
target_link_libraries(nxgenetic nxmemory nxio)

add_executable(nextgen ${MAIN})
target_link_libraries(nextgen nxio nxdependinject nxmemory nxruntime)

install(TARGETS nextgen nxio nxmemory nxconcurrent nxcrypto nxutils nxnetwork nxmutate nxlog nxsyscall nxgenetic nxruntime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

install(CODE "execute_process(COMMAND \"../install.sh\")")
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

include_directories(deps/${UNITY}/src)

add_executable(concurrent-unit-test EXCLUDE_FROM_ALL tests/concurrent/unit/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(concurrent-unit-test nxconcurrent nxio nxmemory)

add_executable(memory-intergration-test EXCLUDE_FROM_ALL tests/memory/intergration/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(memory-intergration-test nxconcurrent)

add_executable(crypto-unit-test EXCLUDE_FROM_ALL tests/crypto/unit/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(crypto-unit-test nxconcurrent nxmemory nxcrypto nxio nxdependinject)

add_executable(utils-unit-test EXCLUDE_FROM_ALL tests/utils/unit/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(utils-unit-test nxconcurrent nxmemory nxutils nxcrypto nxio nxdependinject)

add_executable(resource-integration-test EXCLUDE_FROM_ALL tests/resource/integration/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(resource-integration-test nxnetwork nxdependinject nxio nxconcurrent nxmemory nxutils nxcrypto)

add_executable(mutate-unit-test EXCLUDE_FROM_ALL tests/mutate/unit/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(mutate-unit-test nxmemory nxdependinject nxmutate nxconcurrent nxcrypto)

add_executable(runtime-integration-test EXCLUDE_FROM_ALL tests/runtime/integration/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(runtime-integration-test nxruntime)

add_executable(depend-inject-integration-test EXCLUDE_FROM_ALL tests/depend-inject/integration/tests.c deps/${UNITY}/src/unity.c)
target_link_libraries(depend-inject-integration-test nxmemory nxdependinject nxio nxconcurrent nxcrypto)

add_executable(syscall-unit-test EXCLUDE_FROM_ALL tests/syscall/unit/tests.c ${ENTRY_SOURCES} ${SYSCALL_OS_FILES} src/syscall/syscall.c src/syscall/arg_types.c src/syscall/signals.c src/syscall/generate.c deps/${UNITY}/src/unity.c)
target_link_libraries(syscall-unit-test nxmemory nxdependinject nxio nxcrypto nxconcurrent nxruntime)

add_executable(syscall-integration-test EXCLUDE_FROM_ALL tests/syscall/integration/tests.c ${SYSCALL_OS_FILES} src/syscall/syscall-macos.c src/syscall/arg_types.c src/syscall/signals.c src/syscall/generate.c ${ENTRY_SOURCES} deps/${UNITY}/src/unity.c)
target_link_libraries(syscall-integration-test nxmemory nxdependinject nxio nxconcurrent nxcrypto)

add_sanitizers(depend-inject-integration-test)
add_sanitizers(runtime-integration-test)
add_sanitizers(utils-unit-test)
add_sanitizers(memory-intergration-test)
add_sanitizers(crypto-unit-test)
add_sanitizers(concurrent-unit-test)
add_sanitizers(resource-integration-test)
add_sanitizers(mutate-unit-test)
add_sanitizers(syscall-unit-test)
add_sanitizers(syscall-integration-test)

add_test(depend-inject-integration-test depend-inject-integration-test)
add_test(utils-unit-test utils-unit-test)
add_test(runtime-integration-test runtime-integration-test)
add_test(resource-integration-test resource-integration-test)
add_test(mutate-unit-test mutate-unit-test)
add_test(memory-intergration-test memory-intergration-test)
add_test(crypto-unit-test crypto-unit-test)
add_test(concurrent-unit-test concurrent-unit-test)
add_test(syscall-unit-test syscall-unit-test)
add_test(syscall-integration-test syscall-integration-test)

add_dependencies(check mutate-unit-test)
add_dependencies(check syscall-integration-test)
add_dependencies(check depend-inject-integration-test)
add_dependencies(check utils-unit-test)
add_dependencies(check runtime-integration-test)
add_dependencies(check resource-integration-test)
add_dependencies(check memory-intergration-test)
add_dependencies(check crypto-unit-test)
add_dependencies(check concurrent-unit-test)
add_dependencies(check syscall-unit-test)

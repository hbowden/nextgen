# Set the root directory path, ie the path to the nextgen directory.
ROOT_DIR = $(shell pwd)

COVERAGE = false
ASAN = false

FLAGS = -fsanitize=address -fno-strict-aliasing -fstack-protector-all -Wall -Werror -pedantic -g -O3

CK = ck-0.5.1
LIBRESSL = libressl-2.4.1
CAPSTONE = capstone-3.0.4

INCLUDE = -isystem deps/$(CAPSTONE)/include \
          -isystem deps/$(CK)/include \
          -Isrc

INSTALL = cp -f src/runtime/libnxruntime.so /usr/local/lib && \
	      cp -f src/io/libnxio.so /usr/local/lib && \
	      cp -f src/memory/libnxmemory.so /usr/local/lib && \
	      cp -f src/concurrent/libnxconcurrent.so /usr/local/lib && \
	      cp -f src/crypto/libnxcrypto.so /usr/local/lib && \
	      cp -f src/utils/libnxutils.so /usr/local/lib && \
	      cp -f src/probe/libnxprobe.so /usr/local/lib && \
	      cp -f src/network/libnxnetwork.so /usr/local/lib && \
	      cp -f src/plugins/libnxplugin.so /usr/local/lib && \
	      cp -f src/mutate/libnxmutate.so /usr/local/lib && \
	      cp -f src/resource/libnxresource.so /usr/local/lib && \
	      cp -f src/log/libnxlog.so /usr/local/lib && \
	      cp -f src/syscall/libnxsyscall.so /usr/local/lib && \
	      cp -f src/genetic/libnxgenetic.so /usr/local/lib && \
	      cp -f src/file/libnxfile.so /usr/local/lib && \
	      cp -f src/disas/libnxdisas.so /usr/local/lib && \
	      cp -f deps/$(CAPSTONE)/libcapstone.so /usr/local/lib && \
	      cp -f deps/$(CK)/src/libck.so /usr/local/lib/ && \
	      cp -f deps/$(LIBRESSL)/crypto/.libs/libcrypto.so.38 /usr/local/lib


OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SOURCES = src/main.c

CC = clang

LIB = -rpath src/runtime -rpath src/memory -rpath src/utils \
      -rpath src/io -rpath src/concurrent -rpath src/genetic \
      -rpath src/mutate -rpath src/log -rpath src/network \
      -rpath src/file -rpath src/syscall -rpath src/probe \
      -rpath src/disas -rpath src/crypto -rpath src/plugins \
      -rpath src/resource src/memory/libnxmemory.so \
      src/io/libnxio.so src/runtime/libnxruntime.so \
      src/utils/libnxutils.so \
      src/crypto/libnxcrypto.so \
      src/syscall/libnxsyscall.so \
      deps/$(LIBRESSL)/crypto/.libs/libcrypto.so \
      deps/$(CK)/src/libck.so \
     -l dtrace -l proc -l ctf -l elf -l z -l rtld_db -l pthread -l util

INCLUDE += -I /usr/src/cddl/compat/opensolaris/include \
	       -I /usr/src/cddl/contrib/opensolaris/lib/libdtrace/common/ \
	       -I /usr/src/sys/cddl/compat/opensolaris \
	       -I /usr/src/sys/cddl/contrib/opensolaris/uts/common/

MAKE = gmake

FLAGS += -DFREEBSD -Weverything -std=c99

SILENCED_WARNINGS = -Wno-sign-conversion -Wno-unknown-pragmas -Wno-pedantic -Wno-padded 

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SOURCES = src/main.m

CC = clang

MAKE = make

INSTALL = cp -rf src/runtime/libnxruntime.dylib.dSYM /usr/local/lib && \
	      cp -rf src/io/libnxio.dylib.dSYM /usr/local/lib && \
	      cp -rf src/objc/libnxobjcutils.dylib.dSYM /usr/local/lib && \
	      cp -rf src/memory/libnxmemory.dylib.dSYM /usr/local/lib && \
	      cp -rf src/concurrent/libnxconcurrent.dylib.dSYM /usr/local/lib && \
	      cp -rf src/crypto/libnxcrypto.dylib.dSYM /usr/local/lib && \
	      cp -rf src/probe/libnxprobe.dylib.dSYM /usr/local/lib && \
	      cp -rf src/network/libnxnetwork.dylib.dSYM /usr/local/lib && \
	      cp -rf src/plugins/libnxplugin.dylib.dSYM /usr/local/lib && \
	      cp -rf src/mutate/libnxmutate.dylib.dSYM /usr/local/lib && \
	      cp -rf src/resource/libnxresource.dylib.dSYM /usr/local/lib && \
	      cp -rf src/log/libnxlog.dylib.dSYM /usr/local/lib && \
	      cp -rf src/syscall/libnxsyscall.dylib.dSYM /usr/local/lib && \
	      cp -rf src/genetic/libnxgenetic.dylib.dSYM /usr/local/lib && \
	      cp -rf src/file/libnxfile.dylib.dSYM /usr/local/lib && \
	      cp -rf src/disas/libnxdisas.dylib.dSYM /usr/local/lib && \
	      cp -f src/runtime/libnxruntime.dylib /usr/local/lib && \
	      cp -f src/io/libnxio.dylib /usr/local/lib && \
	      cp -f src/objc/libnxobjcutils.dylib /usr/local/lib && \
	      cp -f src/memory/libnxmemory.dylib /usr/local/lib && \
	      cp -f src/concurrent/libnxconcurrent.dylib /usr/local/lib && \
	      cp -f src/crypto/libnxcrypto.dylib /usr/local/lib && \
	      cp -f src/utils/libnxutils.dylib /usr/local/lib && \
	      cp -f src/probe/libnxprobe.dylib /usr/local/lib && \
	      cp -f src/network/libnxnetwork.dylib /usr/local/lib && \
	      cp -f src/plugins/libnxplugin.dylib /usr/local/lib && \
	      cp -f src/mutate/libnxmutate.dylib /usr/local/lib && \
	      cp -f src/resource/libnxresource.dylib /usr/local/lib && \
	      cp -f src/log/libnxlog.dylib /usr/local/lib && \
	      cp -f src/syscall/libnxsyscall.dylib /usr/local/lib && \
	      cp -f src/genetic/libnxgenetic.dylib /usr/local/lib && \
	      cp -f src/file/libnxfile.dylib /usr/local/lib && \
	      cp -f src/disas/libnxdisas.dylib /usr/local/lib && \
	      cp -f deps/$(CAPSTONE)/libcapstone.dylib /usr/local/lib && \
	      cp -f deps/$(CK)/src/libck.so /usr/local/lib/libck.0.dylib

LIB = src/runtime/libnxruntime.dylib src/memory/libnxmemory.dylib \
      src/utils/libnxutils.dylib src/io/libnxio.dylib src/objc/libnxobjcutils.dylib

FLAGS += -DMAC_OSX -Weverything -std=c99

SILENCED_WARNINGS = -Wno-unknown-warning-option \
                    -Wno-reserved-id-macro \
                    -Wno-incompatible-pointer-types-discards-qualifiers \
                    -Wno-used-but-marked-unused

BUILD_NX_LIBS = cd $(ROOT_DIR)/src/objc && $(MAKE) &&
CLEAN_NX_LIBS = cd $(ROOT_DIR)/src/objc && $(MAKE) clean &&

endif

ifeq ($(OPERATING_SYSTEM), Linux)

SOURCES = src/main.c

CC = gcc

FLAGS += -DLINUX -DCOMMON -Wextra -std=gnu99

LIB = -Wl,-rpath=src/runtime,-rpath=src/memory,-rpath=src/utils,-rpath=$(ROOT_DIR)/src/io,-rpath=src/concurrent,-rpath=src/genetic,-rpath=src/mutate,-rpath=src/log,-rpath=src/network,-rpath=src/file,-rpath=src/syscall,-rpath=src/probe,-rpath=src/disas,-rpath=deps/libressl-2.4.1/crypto/.libs,-rpath=/usr/local/lib,-rpath=src/crypto,-rpath=src/plugins \
      src/memory/libnxmemory.so \
      $(ROOT_DIR)/src/io/libnxio.so \
      src/runtime/libnxruntime.so \
      src/utils/libnxutils.so \
      src/crypto/libnxcrypto.so \
      src/syscall/libnxsyscall.so \
      deps/$(LIBRESSL)/crypto/.libs/libcrypto.so.38 \
      deps/$(CK)/src/libck.so \
      deps/sqlite/sqlite3.so \
      -lpthread \
      -ldl

MAKE = make

endif

PROG = nextgen

BUILD_NX_LIBS += cd $(ROOT_DIR)/src/io && $(MAKE) && \
                 cd $(ROOT_DIR)/src/memory && $(MAKE) && \
                 cd $(ROOT_DIR)/src/concurrent && $(MAKE) && \
                 cd $(ROOT_DIR)/src/crypto && $(MAKE) && \
                 cd $(ROOT_DIR)/src/utils && $(MAKE) && \
                 cd $(ROOT_DIR)/src/probe && $(MAKE) && \
                 cd $(ROOT_DIR)/src/network && $(MAKE) && \
                 cd $(ROOT_DIR)/src/plugins && $(MAKE) && \
                 cd $(ROOT_DIR)/src/mutate && $(MAKE) && \
                 cd $(ROOT_DIR)/src/resource && $(MAKE) && \
                 cd $(ROOT_DIR)/src/log && $(MAKE) && \
                 cd $(ROOT_DIR)/src/syscall && $(MAKE) && \
                 cd $(ROOT_DIR)/src/genetic && $(MAKE) && \
                 cd $(ROOT_DIR)/src/file && $(MAKE) && \
                 cd $(ROOT_DIR)/src/disas && $(MAKE) && \
                 cd $(ROOT_DIR)/src/runtime && $(MAKE)

CLEAN_NX_LIBS += cd $(ROOT_DIR)/src/io && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/memory && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/concurrent && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/crypto && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/utils && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/probe && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/network && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/plugins && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/mutate && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/resource && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/log && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/syscall && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/genetic && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/file && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/disas && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/runtime && $(MAKE) clean

export CK LIBRESSL CAPSTONE ROOT_DIR PLATFORM FLAGS SILENCED_WARNINGS

all:

	$(BUILD_NX_LIBS)

	$(CC) $(FLAGS) $(SOURCES) $(LIB) $(INCLUDE) $(SILENCED_WARNINGS) -o $(PROG)

clean-lib:

	rm -rf src/runtime/libnxruntime.dylib.dSYM
	rm -rf src/io/libnxio.dylib.dSYM
	rm -rf src/objc/libnxobjcutils.dylib.dSYM
	rm -rf src/memory/libnxmemory.dylib.dSYM
	rm -rf src/concurrent/libnxconcurrent.dylib.dSYM
	rm -rf src/crypto/libnxcrypto.dylib.dSYM 
	rm -rf src/probe/libnxprobe.dylib.dSYM 
	rm -rf src/network/libnxnetwork.dylib.dSYM 
	rm -rf src/plugins/libnxplugin.dylib.dSYM 
	rm -rf src/mutate/libnxmutate.dylib.dSYM 
	rm -rf src/resource/libnxresource.dylib.dSYM
	rm -rf src/log/libnxlog.dylib.dSYM 
	rm -rf src/syscall/libnxsyscall.dylib.dSYM
	rm -rf src/genetic/libnxgenetic.dylib.dSYM 
	rm -rf src/file/libnxfile.dylib.dSYM
	rm -rf src/disas/libnxdisas.dylib.dSYM

	rm -f src/runtime/libnxruntime.dylib
	rm -f src/io/libnxio.dylib
	rm -f src/objc/libnxobjcutils.dylib
	rm -f src/memory/libnxmemory.dylib
	rm -f src/concurrent/libnxconcurrent.dylib
	rm -f src/crypto/libnxcrypto.dylib 
	rm -f src/utils/libnxutils.dylib
	rm -f src/probe/libnxprobe.dylib
	rm -f src/network/libnxnetwork.dylib 
	rm -f src/plugins/libnxplugin.dylib
	rm -f src/mutate/libnxmutate.dylib 
	rm -f src/resource/libnxresource.dylib 
	rm -f src/log/libnxlog.dylib
	rm -f src/syscall/libnxsyscall.dylib
	rm -f src/genetic/libnxgenetic.dylib
	rm -f src/file/libnxfile.dylib 
	rm -f src/disas/libnxdisas.dylib

install:

	$(INSTALL)
	cd deps/ck-0.5.1 && make install

clean:

	cd deps/$(LIBRESSL) && $(MAKE) clean;
	cd deps/$(CAPSTONE) && $(MAKE) clean;
	cd deps/$(CK) && $(MAKE) clean;
	$(CLEAN_NX_LIBS)
	rm -rf *dSYM
	rm -rf nextgen
	rm -rf test_suite

build-tests:
	cd tests && $(MAKE) build

test:
	cd tests && $(MAKE) test

test-all:
	cd tests && $(MAKE) test-all


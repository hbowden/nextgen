# Set the root directory path, ie the path to the nextgen directory.
ROOT_DIR = $(shell pwd)

CK = ck-0.5.1
LIBRESSL = libressl-2.3.2
CAPSTONE = capstone-3.0.4

export CK LIBRESSL CAPSTONE

LIB = src/runtime/libnxruntime.dylib src/memory/libnxmemory.dylib \
      src/utils/libnxutils.dylib src/io/libnxio.dylib

INCLUDE = -Ideps/ck-0.5.0/include -Isrc

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SOURCES = src/main.c

CC = clang

MAKE = gmake

FLAGS = -DFREEBSD -Wall -Werror -Weverything -pedantic -g -O3 -std=c99

SILENCED_WARNINGS = -Wno-padded -Wno-reserved-id-macro

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SOURCES = src/main.m

CC = clang

MAKE = make

FLAGS = -DMAC_OSX -fsanitize=address -Wall -Werror -Weverything -pedantic -g -O3 -std=c99

SILENCED_WARNINGS = -Wno-padded -Wno-reserved-id-macro \
                    -Wno-incompatible-pointer-types-discards-qualifiers

BUILD_NX_LIBS = cd $(ROOT_DIR)/src/objc && $(MAKE) &&
CLEAN_NX_LIBS = cd $(ROOT_DIR)/src/objc && $(MAKE) clean &&

LIB += src/objc/libnxobjcutils.dylib

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
                 cd $(ROOT_DIR)/src/reaper && $(MAKE) && \
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
                 cd $(ROOT_DIR)/src/reaper && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/file && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/disas && $(MAKE) clean && \
                 cd $(ROOT_DIR)/src/runtime && $(MAKE) clean

all:

	cd deps/$(CK) && ./configure && $(MAKE);
	cd deps/$(CAPSTONE) && $(MAKE);
	cd deps/$(LIBRESSL) && ./configure && $(MAKE);

	$(BUILD_NX_LIBS)

	$(CC) $(FLAGS) $(SOURCES) $(LIB) $(INCLUDE) $(SILENCED_WARNINGS) -o $(PROG)

quick:

	$(BUILD_NX_LIBS)

	$(CC) $(FLAGS) $(SOURCES) $(LIB) $(INCLUDE) $(SILENCED_WARNINGS) -o $(PROG)

install:

	cp -ipr src/runtime/libnxruntime.dylib.dSYM /usr/local/lib
	cp -ipr src/io/libnxio.dylib.dSYM /usr/local/lib
	cp -ipr src/objc/libnxobjcutils.dylib.dSYM /usr/local/lib
	cp -ipr src/memory/libnxmemory.dylib.dSYM /usr/local/lib
	cp -ipr src/concurrent/libnxconcurrent.dylib.dSYM /usr/local/lib
	cp -ipr src/crypto/libnxcrypto.dylib.dSYM /usr/local/lib 
	cp -ipr src/utils/libnxutils.dylib.dSYM /usr/local/lib
	cp -ipr src/probe/libnxprobe.dylib.dSYM /usr/local/lib 
	cp -ipr src/network/libnxnetwork.dylib.dSYM /usr/local/lib 
	cp -ipr src/plugins/libnxplugin.dylib.dSYM /usr/local/lib 
	cp -ipr src/mutate/libnxmutate.dylib.dSYM /usr/local/lib 
	cp -ipr src/resource/libnxresource.dylib.dSYM /usr/local/lib 
	cp -ipr src/log/libnxlog.dylib.dSYM /usr/local/lib 
	cp -ipr src/syscall/libnxsyscall.dylib.dSYM /usr/local/lib 
	cp -ipr src/genetic/libnxgenetic.dylib.dSYM /usr/local/lib 
	cp -ipr src/reaper/libnxreaper.dylib.dSYM /usr/local/lib 
	cp -ipr src/file/libnxfile.dylib.dSYM /usr/local/lib 
	cp -ipr src/disas/libnxdisas.dylib.dSYM /usr/local/lib 

	cp src/runtime/libnxruntime.dylib /usr/local/lib
	cp src/io/libnxio.dylib /usr/local/lib
	cp src/objc/libnxobjcutils.dylib /usr/local/lib
	cp src/memory/libnxmemory.dylib /usr/local/lib
	cp src/concurrent/libnxconcurrent.dylib /usr/local/lib
	cp src/crypto/libnxcrypto.dylib /usr/local/lib 
	cp src/utils/libnxutils.dylib /usr/local/lib
	cp src/probe/libnxprobe.dylib /usr/local/lib 
	cp src/network/libnxnetwork.dylib /usr/local/lib 
	cp src/plugins/libnxplugin.dylib /usr/local/lib 
	cp src/mutate/libnxmutate.dylib /usr/local/lib 
	cp src/resource/libnxresource.dylib /usr/local/lib 
	cp src/log/libnxlog.dylib /usr/local/lib 
	cp src/syscall/libnxsyscall.dylib /usr/local/lib 
	cp src/genetic/libnxgenetic.dylib /usr/local/lib 
	cp src/reaper/libnxreaper.dylib /usr/local/lib 
	cp src/file/libnxfile.dylib /usr/local/lib 
	cp src/disas/libnxdisas.dylib /usr/local/lib 
	cp deps/$(CAPSTONE)/libcapstone.dylib /usr/local/lib

	cd deps/$(CK) && $(MAKE) install
	cp nextgen /usr/local/bin

clean:

	cd deps/$(LIBRESSL) && $(MAKE) clean;
	cd deps/$(CAPSTONE) && $(MAKE) clean;
	cd deps/$(CK) && $(MAKE) clean;
	$(CLEAN_NX_LIBS)
	rm -rf *dSYM
	rm -rf nextgen
	rm -rf test_suite


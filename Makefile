# Set the root directory path, ie the path to the nextgen directory.
ROOT_DIR = $(shell pwd)

MAKE = make

SOURCES = src/main.c 

CC = clang

FLAGS = -DMAC_OSX -Wall -Werror -Weverything -pedantic -g -O3 -std=c99

SILENCED_WARNINGS = -Wno-padded -Wno-reserved-id-macro

LIB = src/runtime/libnxruntime.dylib src/memory/libnxmemory.dylib \
      src/utils/libnxutils.dylib src/io/libnxio.dylib

INCLUDE = -Ideps/ck-0.5.0/include -Isrc

PROG = nextgen

BUILD_NX_LIBS = cd $(ROOT_DIR)/src/io && $(MAKE) && \
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

CLEAN_NX_LIBS = cd $(ROOT_DIR)/src/io && $(MAKE) clean && \
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

	$(BUILD_NX_LIBS)

	$(CC) $(FLAGS) $(SOURCES) $(LIB) $(INCLUDE) $(SILENCED_WARNINGS) -o $(PROG)

install:

	cp src/runtime/libnxruntime.dylib /usr/local/lib
	cp src/io/libnxio.dylib /usr/local/lib
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
	cp deps/capstone-3.0.4/libcapstone.dylib /usr/local/lib
	cd deps/ck-0.5.0 && $(MAKE) install
	cp nextgen /usr/local/bin

clean:

	$(CLEAN_NX_LIBS)
	rm -rf *dSYM
	rm -rf nextgen
	rm -rf test_suite


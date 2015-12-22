ROOT_DIR = $(shell pwd)

CK = ck-0.5.0

LIBRESSL = libressl-2.3.1

CFLAGS = -Werror -Weverything -Wall -O3 -g

IGNORE = -Wno-reserved-id-macro -Wno-used-but-marked-unused -Wno-format-nonliteral -Wno-padded -Wno-unused-variable \
         -Wno-unused-parameter -Wno-missing-noreturn -Wno-cast-qual -Wno-unused-value -Wno-cast-align -Wno-deprecated-declarations

INCLUDES = -I../ -I../../src -I../../deps/$(CK)/include -I$(ROOT_DIR)/../../deps/$(LIBRESSL)/include/ \
           -I../../src/syscalls/mac/

SOURCES = $(UNIT_TEST).c ../test_utils.c ../../src/generate.c ../../src/network.c ../../src/resource.c \
                         ../../src/concurrent.c ../../src/memory.c ../../src/io.c ../../src/utils.c \
                         ../../src/crypto.c ../../src/arg_types.c ../../src/log.c ../../src/nextgen.c \
                         ../../src/mutate.c ../../src/signals.c ../../src/plugin.c ../../src/shim.c

LIBS = $(ROOT_DIR)/../../deps/$(LIBRESSL)/crypto/.libs/libcrypto.a

OPERATING_SYSTEM = $(shell uname)

UNIT_TEST = test_generate

ifeq ($(OPERATING_SYSTEM), FreeBSD)

CC = clang

endif

ifeq ($(OPERATING_SYSTEM), Darwin)

CC = clang

OS = -DMAC_OSX

ENTRY_SOURCES = ../../src/syscalls/mac/entry_read.c ../../src/syscalls/mac/entry_write.c ../../src/syscalls/mac/entry_open.c \
                ../../src/syscalls/mac/entry_close.c ../../src/syscalls/mac/entry_wait4.c ../../src/syscalls/mac/entry_link.c \
                ../../src/syscalls/mac/entry_unlink.c ../../src/syscalls/mac/entry_chdir.c ../../src/syscalls/mac/entry_fchdir.c \
                ../../src/syscalls/mac/entry_mknod.c ../../src/syscalls/mac/entry_chmod.c ../../src/syscalls/mac/entry_getfsstat.c

endif

all:

	$(CC) $(CFLAGS) $(IGNORE) $(INCLUDES) $(SOURCES) $(LIBS) $(ENTRY_SOURCES) $(OS) -o $(UNIT_TEST) 

.PHONY: clean
clean:

	rm -rf $(UNIT_TEST)
	rm -rf *.dSYM
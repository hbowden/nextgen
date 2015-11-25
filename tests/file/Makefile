CURRENT_DIR = $(shell pwd)

CK = ck-0.5.0

LIBRESSL = libressl-2.3.1

CFLAGS = -Werror -Weverything -Wall -O3 -g

IGNORE = -Wno-reserved-id-macro -Wno-used-but-marked-unused -Wno-format-nonliteral -Wno-padded -Wno-unused-parameter \
         -Wno-missing-noreturn -Wno-unused-variable -Wno-cast-qual

INCLUDES = -I../ -I../../src -I../../deps/$(CK)/include -I$(CURRENT_DIR)/../../deps/$(LIBRESSL)/include/

SOURCES = test_file.c ../test_utils.c ../../src/memory.c ../../src/io.c  \
          ../../src/arg_types.c ../../src/utils.c ../../src/shim.c ../../src/nextgen.c ../../src/crypto.c \
          ../../src/signals.c ../../src/log.c ../../src/concurrent.c ../../src/mutate.c ../../src/plugin.c \
          ../../src/genetic.c ../../src/syscall.c

LIBS = $(CURRENT_DIR)/../../deps/$(LIBRESSL)/crypto/.libs/libcrypto.a

OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), FreeBSD)

CC = clang

endif

ifeq ($(OPERATING_SYSTEM), Darwin)

CC = clang

OS = -DMAC_OSX

OS_FLAGS = -I../../src/syscalls/mac/

ENTRY_SOURCES = ../../src/syscalls/mac/entry_read.c ../../src/syscalls/mac/entry_write.c ../../src/syscalls/mac/entry_open.c \
                ../../src/syscalls/mac/entry_close.c ../../src/syscalls/mac/entry_wait4.c ../../src/syscalls/mac/entry_link.c \
                ../../src/syscalls/mac/entry_unlink.c ../../src/syscalls/mac/entry_chdir.c ../../src/syscalls/mac/entry_fchdir.c \
                ../../src/syscalls/mac/entry_mknod.c ../../src/syscalls/mac/entry_chmod.c ../../src/syscalls/mac/entry_getfsstat.c

endif

all:

	$(CC) $(CFLAGS) $(OS_FLAGS) $(ENTRY_SOURCES) $(IGNORE) $(INCLUDES) $(SOURCES) $(LIBS) $(OS) -o test_file

.PHONY: clean
clean:

	rm -rf test_syscall
	rm -rf *.dSYM
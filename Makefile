
# The core group of source files.
SOURCES = src/nextgen.c src/disas.c src/probe.c src/utils.c src/crypto.c src/runtime.c src/network.c src/syscall.c \
          src/shim.c src/generate.c src/mutate.c src/log.c src/signals.c src/reaper.c src/file.c src/plugin.c src/genetic.c \
          src/memory.c src/concurrent.c src/io.c src/resource.c src/arg_types.c

# Compiler flags passed when make asan is called.
ASAN_FLAGS = -fsanitize=address -Wno-unused-function -DASAN

# Compiler flags passed when make valgrind is called.
VALGRIND_FLAGS = -DVALGRIND

# Dependencies paths.
CK = ck-0.5.0
LIBRESSL = libressl-2.3.1

# Set the root directory path, ie the path to the nextgen directory.
ROOT_DIR = $(shell pwd)

# Set the operating system.
OPERATING_SYSTEM = $(shell uname)

# Set FreeBSD specific variables.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

# Use clang by default on FreeBSD.
CC = clang

# Use gmake on FreeBSD.
MAKE = gmake

# Include directories.
INCLUDES = -I/usr/local/include/ -I/usr/src/cddl/compat/opensolaris/include -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common/ \
           -I/usr/src/sys/cddl/compat/opensolaris -I/usr/src/sys/cddl/contrib/opensolaris/uts/common/ -Isrc/syscalls/freebsd/ \
           -I$(ROOT_DIR)/deps/capstone-3.0.4/ -I$(ROOT_DIR)/deps/capstone-3.0.4/include -I$(ROOT_DIR)/deps/$(CK)/include \
           -I$(ROOT_DIR)/src

# Libraries to link.
LIBS = -lpthread -ldtrace -lproc -lctf -lelf -lz -lrtld_db -lutil -lcrypto deps/capstone-3.0.4/libcapstone.a deps/$(CK)/src/libck.a \
       deps/$(LIBRESSL)/crypto/.libs/libcrypto.a

# Warnings to silence.
SILENCED_WARNINGS = -Wno-incompatible-pointer-types-discards-qualifiers -Wno-used-but-marked-unused -Wno-padded -Wno-unused-parameter \
                    -Wno-unused-variable -Wno-missing-noreturn -Wno-format-nonliteral -Wno-unused-value \
                    -Wno-gnu-statement-expression -Wno-cast-qual -Wno-unknown-pragmas -Wno-sign-conversion -Wno-cast-align

# Compiler flags.
CFLAGS = -DFREEBSD -std=c99 -Werror -Wall -Weverything -Wno-pedantic -g -fstack-protector-all -O3

# Syscall entries C files.
ENTRY_SOURCES = src/syscalls/freebsd/entry_read.c src/syscalls/freebsd/entry_write.c src/syscalls/freebsd/entry_open.c src/syscalls/freebsd/entry_close.c src/syscalls/freebsd/entry_wait4.c \
                src/syscalls/freebsd/entry_creat.c src/syscalls/freebsd/entry_link.c src/syscalls/freebsd/entry_unlink.c src/syscalls/freebsd/entry_chdir.c src/syscalls/freebsd/entry_fchdir.c \
                src/syscalls/freebsd/entry_mknod.c src/syscalls/freebsd/entry_chmod.c src/syscalls/freebsd/entry_getfsstat.c src/syscalls/freebsd/entry_lseek.c src/syscalls/freebsd/entry_setuid.c src/syscalls/freebsd/entry_ptrace.c src/syscalls/freebsd/entry_recvmsg.c

endif

# Mac OSX build settings.
ifeq ($(OPERATING_SYSTEM), Darwin)

CC = clang

MAKE = make

INCLUDES = -i$(ROOT_DIR)/stdatomic.h -I/usr/include -Isrc/syscalls/mac/ -I$(ROOT_DIR)/deps/capstone-3.0.4/ \
           -I$(ROOT_DIR)/deps/capstone-3.0.4/include -I$(ROOT_DIR)/deps/$(CK)/include -I$(ROOT_DIR)/src \
           -I$(ROOT_DIR)/deps/$(LIBRESSL)/include

LIBS = -lpthread -ldtrace -lproc -lz -lutil deps/capstone-3.0.4/libcapstone.a deps/$(LIBRESSL)/crypto/.libs/libcrypto.a

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-used-but-marked-unused -Wno-padded -Wno-unused-parameter \
                    -Wno-unused-variable -Wno-missing-noreturn -Wno-format-nonliteral -Wno-unused-value \
                    -Wno-gnu-statement-expression -Wno-cast-qual -Wno-cast-align -Wno-deprecated-declarations

CFLAGS = -DMAC_OSX -std=c99 -Werror -Wall -Weverything -pedantic -g -fstack-protector-all -O3

ENTRY_SOURCES = src/syscalls/mac/entry_read.c src/syscalls/mac/entry_write.c src/syscalls/mac/entry_open.c \
                src/syscalls/mac/entry_close.c src/syscalls/mac/entry_wait4.c src/syscalls/mac/entry_link.c \
                src/syscalls/mac/entry_unlink.c src/syscalls/mac/entry_chdir.c src/syscalls/mac/entry_fchdir.c \
                src/syscalls/mac/entry_mknod.c src/syscalls/mac/entry_chmod.c src/syscalls/mac/entry_getfsstat.c

endif

ifeq ($(OPERATING_SYSTEM), SunOS)

CC = clang

INCLUDES = -i$(CURRENT_DIR)/stdatomic.h -Isyscalls/SunOS

LIBS = -lpthread -ldtrace -lproc -lz -lutil -lcrypto -lcapstone

SILENCED_WARNINGS = -Wunknown-pragmas

CFLAGS = -DILLUMOS -std=c11 -Wall -Weverything -g -fstack-protector-all -O3 -Wno-documentation -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

ENTRY_SOURCES =

endif

ifeq ($(OPERATING_SYSTEM), Linux)

CC = gcc

MAKE = make

INCLUDES = -I/usr/include -Isrc/syscalls/Linux/ -I$(ROOT_DIR)/deps/capstone-3.0.4/ \
           -I$(ROOT_DIR)/deps/capstone-3.0.4/include -I$(ROOT_DIR)/deps/$(CK)/include -I$(ROOT_DIR)/src \
           -I$(ROOT_DIR)/deps/$(LIBRESSL)/include

LIBS = -lpthread -ldtrace -lproc -lz -lpthread -lutil -lcrypto -lcapstone

CFLAGS = -DLINUX -std=c99 -Wall -Wextra -Wpedantic -g -fstack-protector-all -O3

ENTRY_SOURCES =

endif

all:

	cd $(ROOT_DIR)/deps/$(CK) && ./configure && $(MAKE);
	cd $(ROOT_DIR)/deps/capstone-3.0.4 && $(MAKE);
	cd $(ROOT_DIR)/deps/$(LIBRESSL) && ./configure && $(MAKE)

	$(CC) $(CFLAGS) $(INCLUDES) -o nextgen src/main.c $(SOURCES) $(ENTRY_SOURCES) $(LIBS) $(SILENCED_WARNINGS)

	cd $(ROOT_DIR)/tests/crypto && $(MAKE) 
	cd $(ROOT_DIR)/tests/memory && $(MAKE);
	cd $(ROOT_DIR)/tests/parser && $(MAKE);
	cd $(ROOT_DIR)/tests/genetic && $(MAKE);
	cd $(ROOT_DIR)/tests/reaper && $(MAKE);
	cd $(ROOT_DIR)/tests/resource && $(MAKE);
	cd $(ROOT_DIR)/tests/syscall && $(MAKE);
	cd $(ROOT_DIR)/tests/concurrent && $(MAKE);
	cd $(ROOT_DIR)/tests/file && $(MAKE);
	cd $(ROOT_DIR)/tests/network && $(MAKE);
	cd $(ROOT_DIR)/tests/generate && $(MAKE);

	$(CC) $(CFLAGS) $(INCLUDES) -o test_suite tests/tests.c tests/test_utils.c -Itests $(SOURCES) $(ENTRY_SOURCES) $(LIBS) $(SILENCED_WARNINGS)

.PHONY: install
install:

	mv nextgen /usr/local/bin/

.PHONY: quick
quick:

	$(CC) $(CFLAGS) $(INCLUDES) -o nextgen src/main.c $(SOURCES) $(ENTRY_SOURCES) $(LIBS) $(SILENCED_WARNINGS)

	cd $(ROOT_DIR)/tests/crypto && $(MAKE);
	cd $(ROOT_DIR)/tests/memory && $(MAKE);
	cd $(ROOT_DIR)/tests/parser && $(MAKE);
	cd $(ROOT_DIR)/tests/genetic && $(MAKE);
	cd $(ROOT_DIR)/tests/reaper && $(MAKE);
	cd $(ROOT_DIR)/tests/resource && $(MAKE);
	cd $(ROOT_DIR)/tests/syscall && $(MAKE);
	cd $(ROOT_DIR)/tests/concurrent && $(MAKE);
	cd $(ROOT_DIR)/tests/file && $(MAKE);
	cd $(ROOT_DIR)/tests/network && $(MAKE);
	cd $(ROOT_DIR)/tests/generate && $(MAKE);

.PHONY: asan
asan:

	cd $(ROOT_DIR)/deps/$(CK) && ./configure && $(MAKE);
	cd $(ROOT_DIR)/deps/capstone-3.0.4 && $(MAKE);
	cd $(ROOT_DIR)/deps/$(LIBRESSL) && ./configure && $(MAKE)

	$(CC) $(CFLAGS) $(ASAN_FLAGS) $(INCLUDES) -o nextgen src/main.c $(SOURCES) $(ENTRY_SOURCES) $(LIBS) $(SILENCED_WARNINGS)

.PHONY: valgrind
valgrind:

	cd $(ROOT_DIR)/deps/ck-0.4.5 && ./configure && $(MAKE);
	cd $(ROOT_DIR)/deps/capstone-3.0.4 && $(MAKE);
	cd $(ROOT_DIR)/deps/libressl-2.3.0 && ./configure && $(MAKE)

	$(CC) $(CFLAGS) $(VALGRIND_FLAGS) $(INCLUDES) -o nextgen src/main.c  $(SOURCES) $(ENTRY_SOURCES) $(LIBS) $(SILENCED_WARNINGS)

.PHONY: test
test:

	cd $(ROOT_DIR)/deps/ck-0.4.5 && $(MAKE) check;
	cd $(ROOT_DIR)/deps/capstone-3.0.4 && $(MAKE) check;
	./test_suite;

.PHONY: test-quick
test-quick:

	./test_suite

.PHONY: clean
clean:

	cd $(ROOT_DIR)/deps/capstone-3.0.4 && $(MAKE) clean
	cd $(ROOT_DIR)/deps/$(LIBRESSL) && $(MAKE) clean
	cd deps/$(CK) && $(MAKE) clean
	rm -rf nextgen
	rm -rf test_suite
	rm -rf *.dSYM
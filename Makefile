
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
CURRENT_DIR = $(shell pwd)

# Set the operating system.
OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), FreeBSD)

CC = clang

MAKE = gmake

INCLUDES = -I/usr/local/include/ -I/usr/src/cddl/compat/opensolaris/include -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common/ \
           -I/usr/src/sys/cddl/compat/opensolaris -I/usr/src/sys/cddl/contrib/opensolaris/uts/common/ -Isrc/syscalls/freebsd/ \
           -I$(CURRENT_DIR)/deps/capstone-3.0.4/ -I$(CURRENT_DIR)/deps/capstone-3.0.4/include -I$(CURRENT_DIR)/deps/$(CK)/include \
           -I$(CURRENT_DIR)/src

LIBS = -lpthread -ldtrace -lproc -lctf -lelf -lz -lrtld_db -lutil -lcrypto deps/capstone-3.0.4/libcapstone.a deps/$(CK)/src/libck.a \
       deps/$(LIBRESSL)/crypto/.libs/libcrypto.a

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-used-but-marked-unused -Wno-padded -Wno-unused-parameter \
                    -Wno-unused-variable -Wno-missing-noreturn -Wno-format-nonliteral -Wno-unused-value \
                    -Wno-gnu-statement-expression -Wno-cast-qual -Wno-unknown-pragmas -Wno-sign-conversion

CFLAGS = -DFREEBSD -std=c99 -Werror -Wall -Weverything -Wno-pedantic -g -fstack-protector-all -O3

ENTRY_SOURCES = src/syscalls/freebsd/entry_read.c src/syscalls/freebsd/entry_write.c src/syscalls/freebsd/entry_open.c src/syscalls/freebsd/entry_close.c src/syscalls/freebsd/entry_wait4.c \
                src/syscalls/freebsd/entry_creat.c src/syscalls/freebsd/entry_link.c src/syscalls/freebsd/entry_unlink.c src/syscalls/freebsd/entry_chdir.c src/syscalls/freebsd/entry_fchdir.c \
                src/syscalls/freebsd/entry_mknod.c src/syscalls/freebsd/entry_chmod.c src/syscalls/freebsd/entry_getfsstat.c src/syscalls/freebsd/entry_lseek.c src/syscalls/freebsd/entry_setuid.c src/syscalls/freebsd/entry_ptrace.c src/syscalls/freebsd/entry_recvmsg.c

endif

ifeq ($(OPERATING_SYSTEM), Darwin)

CC = clang

MAKE = make

INCLUDES = -i$(CURRENT_DIR)/stdatomic.h -I/usr/include -Isrc/syscalls/mac/ -I$(CURRENT_DIR)/deps/capstone-3.0.4/ \
           -I$(CURRENT_DIR)/deps/capstone-3.0.4/include -I$(CURRENT_DIR)/deps/$(CK)/include -I$(CURRENT_DIR)/src \
           -I$(CURRENT_DIR)/deps/$(LIBRESSL)/include

LIBS = -lpthread -ldtrace -lproc -lz -lutil deps/capstone-3.0.4/libcapstone.a deps/$(LIBRESSL)/crypto/.libs/libcrypto.a

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-used-but-marked-unused -Wno-padded -Wno-unused-parameter \
                    -Wno-unused-variable -Wno-missing-noreturn -Wno-format-nonliteral -Wno-unused-value \
                    -Wno-gnu-statement-expression -Wno-cast-qual

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

INCLUDES = -I/usr/include -Isrc/syscalls/Linux/ -I$(CURRENT_DIR)/deps/capstone-3.0.4/ \
           -I$(CURRENT_DIR)/deps/capstone-3.0.4/include -I$(CURRENT_DIR)/deps/$(CK)/include -I$(CURRENT_DIR)/src \
           -I$(CURRENT_DIR)/deps/$(LIBRESSL)/include

LIBS = -lpthread -ldtrace -lproc -lz -lpthread -lutil -lcrypto -lcapstone

CFLAGS = -DLINUX -std=c99 -Wall -Wextra -Wpedantic -g -fstack-protector-all -O3

ENTRY_SOURCES =

endif

all:

	cd $(CURRENT_DIR)/deps/$(CK) && ./configure && $(MAKE);
	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && $(MAKE);
	cd $(CURRENT_DIR)/deps/$(LIBRESSL) && ./configure && $(MAKE)

	$(CC) $(CFLAGS) $(INCLUDES) -o nextgen src/main.c $(SOURCES) $(ENTRY_SOURCES) $(LIBS) $(SILENCED_WARNINGS)

	cd $(CURRENT_DIR)/tests/crypto && $(MAKE);
	cd $(CURRENT_DIR)/tests/memory && $(MAKE);
	cd $(CURRENT_DIR)/tests/parser && $(MAKE);
	cd $(CURRENT_DIR)/tests/genetic && $(MAKE);
	cd $(CURRENT_DIR)/tests/reaper && $(MAKE);
	cd $(CURRENT_DIR)/tests/resource && $(MAKE);
	cd $(CURRENT_DIR)/tests/syscall && $(MAKE);
	cd $(CURRENT_DIR)/tests/concurrent && $(MAKE);
	cd $(CURRENT_DIR)/tests/file && $(MAKE);

	$(CC) $(CFLAGS) $(INCLUDES) -o test_suite tests/tests.c tests/test_utils.c -Itests $(SOURCES) $(ENTRY_SOURCES) $(LIBS) $(SILENCED_WARNINGS)

.PHONY: install
install:

	mv nextgen /usr/local/bin/

.PHONY: quick
quick:

	$(CC) $(CFLAGS) $(INCLUDES) -o nextgen src/main.c $(SOURCES) $(ENTRY_SOURCES) $(LIBS) $(SILENCED_WARNINGS)

	cd $(CURRENT_DIR)/tests/crypto && $(MAKE);
	cd $(CURRENT_DIR)/tests/memory && $(MAKE);
	cd $(CURRENT_DIR)/tests/parser && $(MAKE);
	cd $(CURRENT_DIR)/tests/genetic && $(MAKE);
	cd $(CURRENT_DIR)/tests/reaper && $(MAKE);
	cd $(CURRENT_DIR)/tests/resource && $(MAKE);
	cd $(CURRENT_DIR)/tests/syscall && $(MAKE);
	cd $(CURRENT_DIR)/tests/concurrent && $(MAKE);
	cd $(CURRENT_DIR)/tests/file && $(MAKE);

.PHONY: asan
asan:

	cd $(CURRENT_DIR)/deps/$(CK) && ./configure && $(MAKE);
	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && $(MAKE);
	cd $(CURRENT_DIR)/deps/$(LIBRESSL) && ./configure && $(MAKE)

	$(CC) $(CFLAGS) $(ASAN_FLAGS) $(INCLUDES) -o nextgen src/main.c $(SOURCES) $(ENTRY_SOURCES) $(LIBS) $(SILENCED_WARNINGS)

.PHONY: valgrind
valgrind:

	cd $(CURRENT_DIR)/deps/ck-0.4.5 && ./configure && $(MAKE);
	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && $(MAKE);
	cd $(CURRENT_DIR)/deps/libressl-2.3.0 && ./configure && $(MAKE)

	$(CC) $(CFLAGS) $(VALGRIND_FLAGS) $(INCLUDES) -o nextgen src/main.c  $(SOURCES) $(ENTRY_SOURCES) $(LIBS) $(SILENCED_WARNINGS)

.PHONY: test
test:

	cd $(CURRENT_DIR)/deps/ck-0.4.5 && $(MAKE) check;
	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && $(MAKE) check;
	./test_suite;

.PHONY: test-quick
test-quick:

	./test_suite

.PHONY: clean
clean:

	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && $(MAKE) clean
	cd $(CURRENT_DIR)/deps/$(LIBRESSL) && $(MAKE) clean
	cd deps/$(CK) && $(MAKE) clean
	rm -rf nextgen
	rm -rf test_suite
	rm -rf *.dSYM
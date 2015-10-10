
SOURCES = src/nextgen.c src/disas.c src/probe.c src/utils.c src/crypto.c src/runtime.c src/child.c src/network.c src/syscall.c src/shim.c src/generate.c src/mutate.c src/log.c src/signals.c src/reaper.c src/file.c src/plugin.c src/genetic.c src/memory.c src/concurrent.c src/io.c src/resource.c src/arg_types.c

ASAN_FLAGS = -fsanitize=address -DASAN

VALGRIND_FLAGS = -DVALGRIND

TEST_FLAGS =

TEST_SOURCES = tests/tests.c tests/test_map.c

CURRENT_DIR = $(shell pwd)

OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), FreeBSD)

CC = clang

MAKE = gmake

INCLUDES = -I/usr/src/cddl/compat/opensolaris/include -I/usr/local/include/ -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common/ -I/usr/src/sys/cddl/compat/opensolaris -I/usr/src/sys/cddl/contrib/opensolaris/uts/common/ -Isrc/syscalls/freebsd/ -I$(CURRENT_DIR)/deps/capstone-3.0.4/ -I$(CURRENT_DIR)/deps/capstone-3.0.4/include -I$(CURRENT_DIR)/deps/ck-0.4.5/include -I$(CURRENT_DIR)/src

LIBS = -lpthread -lstdthreads -ldtrace -lproc -lctf -lelf -lz -lrtld_db -lutil -lcrypto deps/capstone-3.0.4/libcapstone.a deps/ck-0.4.5/src/libck.a

CFLAGS = -DFREEBSD -std=c11 -Wall -Werror -Wno-extra-tokens -Wno-unused-value -Weverything -g -fstack-protector-all -Wno-cast-qual -Wno-missing-noreturn -Wno-incompatible-pointer-types-discards-qualifiers -Wno-unused-parameter -Wno-unused-variable -Wno-used-but-marked-unused -Wno-unreachable-code-return -Wno-reserved-id-macro -Wno-documentation -Wno-disabled-macro-expansion -Wno-switch-enum -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

ENTRY_SOURCES = src/syscalls/freebsd/entry_read.c src/syscalls/freebsd/entry_write.c src/syscalls/freebsd/entry_open.c src/syscalls/freebsd/entry_close.c src/syscalls/freebsd/entry_wait4.c \
                src/syscalls/freebsd/entry_creat.c src/syscalls/freebsd/entry_link.c src/syscalls/freebsd/entry_unlink.c src/syscalls/freebsd/entry_chdir.c src/syscalls/freebsd/entry_fchdir.c \
                src/syscalls/freebsd/entry_mknod.c src/syscalls/freebsd/entry_chmod.c src/syscalls/freebsd/entry_getfsstat.c src/syscalls/freebsd/entry_lseek.c src/syscalls/freebsd/entry_setuid.c src/syscalls/freebsd/entry_ptrace.c src/syscalls/freebsd/entry_recvmsg.c

endif

ifeq ($(OPERATING_SYSTEM), Darwin)

CC = clang

MAKE = make

INCLUDES = -i$(CURRENT_DIR)/stdatomic.h -Isrc/syscalls/mac/ -I$(CURRENT_DIR)/deps/capstone-3.0.4/ \
           -I$(CURRENT_DIR)/deps/capstone-3.0.4/include -I$(CURRENT_DIR)/deps/ck-0.4.5/include -I$(CURRENT_DIR)/src \
           -I$(CURRENT_DIR)/deps/libressl-2.3.0/include

LIBS = -lpthread -ldtrace -lproc -lz -lutil deps/capstone-3.0.4/libcapstone.a deps/libressl-2.3.0/crypto/.libs/libcrypto.a

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

CFLAGS = -DILLUMOS -std=c11 -Wall -Weverything -g -fstack-protector-all -O3 -Wno-documentation -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

ENTRY_SOURCES =

endif

ifeq ($(OPERATING_SYSTEM), Linux)

CC = gcc

INCLUDES = -i$(CURRENT_DIR)/stdatomic.h -Isyscalls/SunOS

LIBS = -lpthread -ldtrace -lproc -lz -lpthread -lutil -lcrypto -lcapstone

CFLAGS = -DLINUX -std=c11 -Wall -Weverything -g -fstack-protector-all -O3 -Wno-documentation -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

ENTRY_SOURCES =

endif

all:

	cd $(CURRENT_DIR)/deps/ck-0.4.5 && ./configure && $(MAKE);
	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && $(MAKE);
	cd $(CURRENT_DIR)/deps/splint-3.1.2 && ./configure && $(MAKE);
	cd $(CURRENT_DIR)/deps/libressl-2.3.0 && ./configure && $(MAKE)

	$(CC) $(CFLAGS) $(INCLUDES) -o nextgen $(SOURCES) $(ENTRY_SOURCES) $(LIBS) $(SILENCED_WARNINGS)

.PHONY: install
install:

	mv nextgen /usr/local/bin/

.PHONY: asan
asan:

	cd $(CURRENT_DIR)/deps/ck-0.4.5 && ./configure && $(MAKE);
	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && $(MAKE);

	$(CC) $(CFLAGS) $(ASAN_FLAGS) $(INCLUDES) -o nextgen $(SOURCES) $(ENTRY_SOURCES) $(LIBS)

.PHONY: valgrind
valgrind:

	cd $(CURRENT_DIR)/deps/ck-0.4.5 && ./configure && $(MAKE);
	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && $(MAKE);

	$(CC) $(CFLAGS) $(VALGRIND_FLAGS) $(INCLUDES) -o nextgen $(SOURCES) $(ENTRY_SOURCES) $(LIBS)

.PHONY: splint
splint:

	$(CURRENT_DIR)/deps/splint-3.1.2/bin/splint src/*.h
	$(CURRENT_DIR)/deps/splint-3.1.2/bin/splint src/*.c

.PHONY: test
test:

	$(CC) $(TEST_FLAGS) $(TEST_SOURCES) $(INCLUDES) -o test_suite
	cd $(CURRENT_DIR)/deps/ck-0.4.5 && $(MAKE) check;
	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && $(MAKE) check;
	./test_suite;

.PHONY: clean
clean:

	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && $(MAKE) clean
	cd deps/ck-0.4.5 && $(MAKE) clean
	cd $(CURRENT_DIR)/deps/splint-3.1.2 && $(MAKE) clean
	cd $(CURRENT_DIR)/deps/libressl-2.3.0 && $(MAKE) clean
	rm nextgen
	rm /usr/local/bin/nextgen

SOURCES = src/nextgen.c src/disas.c src/probe.c src/utils.c src/crypto.c src/runtime.c src/child.c src/network.c src/syscall.c src/shim.c src/generate.c src/mutate.c src/log.c src/signals.c src/reaper.c

TEST_FLAGS =

TEST_SOURCES = tests/tests.c tests/test_map.c

CURRENT_DIR = $(shell pwd)

OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), FreeBSD)

CC = clang

INCLUDES = -I/usr/src/cddl/compat/opensolaris/include -I/usr/local/include/ -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common/ -I/usr/src/sys/cddl/compat/opensolaris -I/usr/src/sys/cddl/contrib/opensolaris/uts/common/ -Isrc/syscalls/freebsd/ -I$(CURRENT_DIR)/deps/capstone-3.0.4/ -I$(CURRENT_DIR)/deps/capstone-3.0.4/include -I$(CURRENT_DIR)/deps/ck-0.4.5/include -I$(CURRENT_DIR)/src

LIBS = -lpthread -ldtrace -lproc -lctf -lelf -lz -lrtld_db -lpthread -lutil -lcrypto deps/capstone-3.0.4/libcapstone.a deps/ck-0.4.5/src/libck.a

CFLAGS = -DFREEBSD -O3 -std=c11 -Wall -Weverything -g -fstack-protector-all -Wno-cast-qual -Wno-missing-noreturn -Wno-unreachable-code-return -Wno-reserved-id-macro -Wno-documentation -Wno-disabled-macro-expansion -Wno-switch-enum -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

ENTRY_SOURCES = src/syscalls/freebsd/entry_read.c src/syscalls/freebsd/entry_write.c src/syscalls/freebsd/entry_open.c src/syscalls/freebsd/entry_close.c src/syscalls/freebsd/entry_wait4.c src/syscalls/freebsd/entry_creat.c src/syscalls/freebsd/entry_link.c src/syscalls/freebsd/entry_unlink.c src/syscalls/freebsd/entry_chdir.c src/syscalls/freebsd/entry_fchdir.c src/syscalls/freebsd/entry_mknod.c src/syscalls/freebsd/entry_chmod.c src/syscalls/freebsd/entry_getfsstat.c

endif

ifeq ($(OPERATING_SYSTEM), Darwin)

CC = clang

INCLUDES = -i$(CURRENT_DIR)/stdatomic.h -Isyscalls/mac

LIBS = -lpthread -ldtrace -lproc -lz -lpthread -lutil -lcrypto -lcapstone

CFLAGS = -DMAC_OSX -std=c11 -Wall -Weverything -g -fstack-protector-all -O3 -Wno-documentation -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

ENTRY_SOURCES = syscalls/mac/entry_read.c syscalls/mac/entry_write.c syscalls/mac/entry_open.c syscalls/mac/entry_close.c syscalls/mac/entry_wait4.c syscalls/mac/entry_link.c syscalls/mac/entry_unlink.c syscalls/mac/entry_chdir.c syscalls/mac/entry_fchdir.c syscalls/mac/entry_mknod.c syscalls/mac/entry_chmod.c syscalls/mac/entry_getfsstat.c

endif

ifeq ($(OPERATING_SYSTEM), SunOS)

CC = clang

INCLUDES = -i$(CURRENT_DIR)/stdatomic.h -Isyscalls/SunOS

LIBS = -lpthread -ldtrace -lproc -lz -lpthread -lutil -lcrypto -lcapstone

CFLAGS = -DILLUMOS -std=c11 -Wall -Weverything -g -fstack-protector-all -O3 -Wno-documentation -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

ENTRY_SOURCES =

endif

ifeq ($(OPERATING_SYSTEM), Linux)

CC = clang

INCLUDES = -i$(CURRENT_DIR)/stdatomic.h -Isyscalls/SunOS

LIBS = -lpthread -ldtrace -lproc -lz -lpthread -lutil -lcrypto -lcapstone

CFLAGS = -DLINUX -std=c11 -Wall -Weverything -g -fstack-protector-all -O3 -Wno-documentation -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

ENTRY_SOURCES =

endif

all:

	cd $(CURRENT_DIR)/deps/ck-0.4.5 && ./configure && gmake;
	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && gmake;

	$(CC) $(CFLAGS) $(INCLUDES) -o nextgen $(SOURCES) $(ENTRY_SOURCES) $(LIBS)

.PHONY: install
install:

	mv nextgen /usr/local/bin/

.PHONY: test
test:

	$(CC) $(TEST_FLAGS) $(TEST_SOURCES) $(INCLUDES) -o test_suite
	cd $(CURRENT_DIR)/deps/ck-0.4.5 && gmake check;
	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && gmake check;
	./test_suite;

.PHONY: clean
clean:

	cd $(CURRENT_DIR)/deps/capstone-3.0.4 && gmake clean
	cd deps/ck-0.4.5 && gmake clean
	rm nextgen
	rm /usr/local/bin/nextgen

SOURCES = nextgen.c disas.c probe.c utils.c crypto.c runtime.c reaper.c child.c network.c syscall.c shim.c signal.c generate.c

CURRENT_DIR = $(shell pwd)

OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), FreeBSD)

CC = clang

INCLUDES = -I/usr/src/cddl/compat/opensolaris/include -I/usr/local/include/ -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common/ -I/usr/src/sys/cddl/compat/opensolaris -I/usr/src/sys/cddl/contrib/opensolaris/uts/common/ -Isyscalls/freebsd/

LIBS = -lpthread -ldtrace -lproc -lctf -lelf -lz -lrtld_db -lpthread -lutil -lcrypto -lcapstone

CFLAGS = -DFREEBSD -O3 -std=c11 -fsanitize=address -Wall -Weverything -g -fstack-protector-all -Wno-missing-noreturn -Wno-unreachable-code-return -Wno-reserved-id-macro -Wno-documentation -Wno-disabled-macro-expansion -Wno-switch-enum -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

ENTRY_SOURCES = syscalls/freebsd/entry_read.c syscalls/freebsd/entry_write.c syscalls/freebsd/entry_open.c syscalls/freebsd/entry_close.c syscalls/freebsd/entry_wait4.c syscalls/freebsd/entry_creat.c syscalls/freebsd/entry_link.c syscalls/freebsd/entry_unlink.c syscalls/freebsd/entry_chdir.c syscalls/freebsd/entry_fchdir.c syscalls/freebsd/entry_mknod.c syscalls/freebsd/entry_chmod.c syscalls/freebsd/entry_getfsstat.c

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

	$(CC) $(CFLAGS) $(INCLUDES) -o nextgen $(SOURCES) $(ENTRY_SOURCES) $(LIBS)

.PHONY: install
install:

	mv nextgen /usr/local/bin/

.PHONY: clean
clean:

	rm nextgen
	rm /usr/local/bin/nextgen
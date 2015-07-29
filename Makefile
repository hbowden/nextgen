
SOURCES = nextgen.c disas.c probe.c utils.c crypto.c runtime.c reaper.c child.c

OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), FreeBSD)

CC = clang

INCLUDES = -I/usr/src/cddl/compat/opensolaris/include -I/usr/local/include/ -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common/ -I/usr/src/sys/cddl/compat/opensolaris -I/usr/src/sys/cddl/contrib/opensolaris/uts/common/ -I/usr/home/nah/distorm-read-only/include/distorm.h

LIBS = -lpthread -ldtrace -lproc -lctf -lelf -lz -lrtld_db -lpthread -lutil -lcrypto -lcapstone

CFLAGS = -DFREEBSD -O3 -std=c11 -fsanitize=address -Wall -Weverything -g -fstack-protector-all -Wno-reserved-id-macro -Wno-documentation -Wno-disabled-macro-expansion -Wno-switch-enum -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral 

endif

ifeq ($(OPERATING_SYSTEM), Darwin)

CFLAGS = -DMAC_OSX -Wall -Werror -Weverything -g -fstack-protector-all -O3 -Wno-documentation -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

LIBS = -lpthread -ldtrace -lproc -lz -lpthread -lutil libmacho/libmacho.a /usr/local/lib/libcrypto.dylib /usr/local/lib/libdistorm3.dylib

INCLUDES =

endif

all:

	$(CC) $(CFLAGS) $(INCLUDES) -o nextgen $(SOURCES) $(LIBS)

.PHONY: install
install:

.PHONY: clean
clean:

	rm nextgen
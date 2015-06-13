

SOURCES = nextgen.c disas.c probe.c utils.c crypto.c runtime.c reaper.c

INCLUDES = -I/usr/src/cddl/compat/opensolaris/include -I/usr/local/include/ -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common/ -I/usr/src/sys/cddl/compat/opensolaris -I/usr/src/sys/cddl/contrib/opensolaris/uts/common/ -I/usr/home/nah/distorm-read-only/include/distorm.h

LIBS = -lpthread -ldtrace -lproc -lctf -lelf -lz -lrtld_db -lpthread -lutil -lcrypto /usr/local/lib/libdistorm3.dylib

OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), Darwin)

CFLAGS = -DMAC_OSX -Wall -Werror -Weverything -g -fstack-protector-all -O3 -Wno-documentation -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

LIBS = -lpthread -ldtrace -lproc  -lz -lpthread -lutil /usr/local/lib/libcrypto.dylib /usr/local/lib/libdistorm3.dylib

endif


all:
	$(CC) $(CFLAGS) $(INCLUDES) -o nextgen $(SOURCES) $(LIBS)

.PHONY: install
install:


.PHONY: clean
clean:

	rm nextgen
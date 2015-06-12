

SOURCES = nextgen.c disas.c probe.c utils.c crypto.c runtime.c reaper.c

INCLUDES = -I/usr/src/cddl/compat/opensolaris/include -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common/ -I/usr/src/sys/cddl/compat/opensolaris -I/usr/src/sys/cddl/contrib/opensolaris/uts/common/ -I/usr/home/nah/distorm-read-only/include/distorm.h

LIBS = -lpthread -ldtrace -lproc -lctf -lelf -lz -lrtld_db -lpthread -lutil -lcrypto /usr/local/lib/libdistorm3.so

CFLAGS = -Wall -Werror -Weverything -g -fstack-protector-all -O3 -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-reserved-id-macro -Wno-sign-conversion -Wno-unknown-pragmas -Wno-format-nonliteral

all:
	$(CC) $(CFLAGS) $(INCLUDES) -o nextgen $(SOURCES) $(LIBS)

.PHONY: install
install:


.PHONY: clean
clean:

	rm nextgen


SOURCES = nextgen.c disas.c probe.c

INCLUDES = -I/usr/src/cddl/compat/opensolaris/include -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common/ -I/usr/src/sys/cddl/compat/opensolaris -I/usr/src/sys/cddl/contrib/opensolaris/uts/common/

LIBS = -lpthread -ldtrace -lproc -lctf -lelf -lz -lrtld_db -lpthread -lutil -ldistorm3

DPATH =

CFLAGS = -Wall -Werror -Weverything -g -fstack-protector-all -O3 -Wno-newline-eof -Wno-padded -Wno-pedantic -Wno-reserved-id-macro -Wno-sign-conversion -Wno-unknown-pragmas

all:

	# Now lets compile the rest.
	$(CC) $(CFLAGS) $(INCLUDES) -o nextgen $(SOURCES) $(LIBS) $(DPATH)

.PHONY: install
install:


.PHONY: clean
clean:

	rm nextgen
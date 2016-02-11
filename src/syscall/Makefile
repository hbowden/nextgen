SOURCES = syscall.c syscall-mac.c arg_types.c generate.c \
          signals.c

ENTRY_SOURCES := $(wildcard mac/*.c)

INCLUDES = -I../ -I../../deps/ck-0.5.0/include -Imac
           
SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-used-but-marked-unused -Wno-cast-align \
                    -Wno-missing-noreturn -Wno-unused-parameter \
                    -Wno-deprecated-declarations

FLAGS = -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../crypto/libnxcrypto.dylib ../concurrent/libnxconcurrent.dylib \
      ../network/libnxnetwork.dylib ../utils/libnxutils.dylib \
      ../probe/libnxprobe.dylib ../resource/libnxresource.dylib \
      ../mutate/libnxmutate.dylib ../log/libnxlog.dylib 

LIB_NAME = libnxsyscall.dylib

all:

	$(CC) -dynamiclib -DMAC_OSX $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(ENTRY_SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
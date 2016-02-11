
SOURCES = file.c file-mac.m

INCLUDES = -I../ -I../../deps/ck-0.5.0/include

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded -Wno-cast-qual \
                    -Wno-unused-parameter

FLAGS = -Wall -Werror -Weverything -Wpedantic -g -O3

LIB = -lobjc ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../utils/libnxutils.dylib ../concurrent/libnxconcurrent.dylib \
      ../mutate/libnxmutate.dylib ../log/libnxlog.dylib \
      ../crypto/libnxcrypto.dylib

LIB_NAME = libnxfile.dylib

all:

	$(CC) -dynamiclib -DMAC_OSX $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
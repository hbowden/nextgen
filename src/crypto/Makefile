SOURCES = crypto.c

INCLUDES = -I../ -I../../deps/libressl-2.3.1/include \
           -I../../deps/ck-0.5.0/include
           
SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded

FLAGS = -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../../deps/libressl-2.3.1/crypto/.libs/libcrypto.a

LIB_NAME = libnxcrypto.dylib

all:

	$(CC) -dynamiclib -DMAC_OSX $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
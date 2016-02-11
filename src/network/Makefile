SOURCES = network.c

INCLUDES = -I../ -I../../deps/ck-0.5.0/include
           
SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded

FLAGS = -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../crypto/libnxcrypto.dylib

LIB_NAME = libnxnetwork.dylib

all:

	$(CC) -dynamiclib -DMAC_OSX $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
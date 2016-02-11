
SOURCES = memory.c

INCLUDES = -I../ -I../../deps/ck-0.5.0/include

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded -Wno-cast-qual \
                    -Wno-used-but-marked-unused

FLAGS = -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib

LIB_NAME = libnxmemory.dylib

all:

	$(CC) -dynamiclib -DMAC_OSX $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
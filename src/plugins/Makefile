SOURCES = plugin.c

INCLUDES = -I../ -I../../deps/ck-0.5.0/include
           
SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-unused-parameter -Wno-cast-qual \

FLAGS = -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../utils/libnxutils.dylib
      
LIB_NAME = libnxplugin.dylib

all:

	$(CC) -dynamiclib -DMAC_OSX $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
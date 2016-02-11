SOURCES = disas.c disas-mac.c

INCLUDES = -I../ -I../../deps/ck-0.5.0/include \
           -I../../deps/capstone-3.0.4/include
           
SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-unused-parameter

FLAGS = -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../file/libnxfile.dylib

LIB_NAME = libnxdisas.dylib ../../deps/capstone-3.0.4/libcapstone.a

all:

	$(CC) -dynamiclib -DMAC_OSX $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
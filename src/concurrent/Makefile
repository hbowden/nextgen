
SOURCES = concurrent.c

INCLUDES = -I../ -I../../deps/ck-0.5.0/include

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-format-nonliteral -Wno-gnu-statement-expression

FLAGS = -Wall -Werror -Weverything -g -O3 -std=c99

LIB = ../../deps/ck-0.5.0/src/libck.so ../io/libnxio.dylib

LIB_NAME = libnxconcurrent.dylib

all:

	$(CC) -dynamiclib -DMAC_OSX $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
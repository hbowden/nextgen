
SOURCES = concurrent.c

INCLUDES = -I../ -I../../deps/ck-0.5.0/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-format-nonliteral -Wno-gnu-statement-expression \
                    -Wno-unused-parameter -Wno-undef

FLAGS = -DFREEBSD -shared -fPIC -Wall -Werror -Weverything -g -O3 -std=c99

LIB = ../../deps/ck-0.5.0/src/libck.so ../io/libnxio.so

LIB_NAME = libnxconcurrent.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-format-nonliteral -Wno-gnu-statement-expression

FLAGS = -dynamiclib -DMAC_OSX -Wall -Werror -Weverything -g -O3 -std=c99

LIB = ../../deps/ck-0.5.0/src/libck.so ../io/libnxio.dylib

LIB_NAME = libnxconcurrent.dylib

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
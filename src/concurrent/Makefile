
SOURCES = concurrent.c

INCLUDES = -I../ -I../../deps/ck-0.5.0/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SOURCES += concurrent-freebsd.c

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-format-nonliteral -Wno-gnu-statement-expression \
                    -Wno-unused-parameter -Wno-undef -Wno-unused-parameter

FLAGS = -DFREEBSD -shared -fPIC -Wall -Werror -Weverything -g -O3 -std=c99

LIB = ../../deps/ck-0.5.0/src/libck.so ../io/libnxio.so

LIB_NAME = libnxconcurrent.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SOURCES += concurrent-mac.c

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-format-nonliteral -Wno-gnu-statement-expression \
                    -Wno-unused-parameter

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
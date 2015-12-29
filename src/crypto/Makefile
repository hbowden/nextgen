SOURCES = crypto.c

INCLUDES = -I../ -I../../deps/libressl-2.3.1/include \
           -I../../deps/ck-0.5.0/include 

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded

FLAGS = -DFREEBSD -shared -fPIC -Wall -Werror -Weverything -g -O3 -Wpedantic -std=c99

LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../../deps/libressl-2.3.1/crypto/.libs/libcrypto.so

LIB_NAME = libnxcrypto.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded

FLAGS = -dynamiclib -DMAC_OSX -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../../deps/libressl-2.3.1/crypto/.libs/libcrypto.a

LIB_NAME = libnxcrypto.dylib

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
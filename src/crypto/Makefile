SOURCES = $(ROOT_DIR)/src/crypto/crypto.c

INCLUDES = -I../ -I../../deps/$(LIBRESSL)/include \
           -isystem ../../deps/$(LIBRESSL)/include \
           -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SILENCED_WARNINGS =

FLAGS = -DFREEBSD -shared -fno-strict-aliasing -fstack-protector-all -fPIC -Wall -Werror -Weverything -g -O3 -Wpedantic -std=c99

LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../../deps/$(LIBRESSL)/crypto/.libs/libcrypto.so

LIB_NAME = libnxcrypto.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SILENCED_WARNINGS = -Wno-unknown-warning-option \
                    -Wno-reserved-id-macro

FLAGS = -dynamiclib -DMAC_OSX -fno-strict-aliasing -fstack-protector-all -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../../deps/$(LIBRESSL)/crypto/.libs/libcrypto.a

LIB_NAME = libnxcrypto.dylib

endif

ifeq ($(OPERATING_SYSTEM), Linux)

SILENCED_WARNINGS =

FLAGS += -shared -fPIC -fstack-protector-all
LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../../deps/$(LIBRESSL)/crypto/.libs/libcrypto.so

LIB_NAME = libnxcrypto.so

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM

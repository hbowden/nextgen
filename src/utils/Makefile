SOURCES = $(ROOT_DIR)/src/utils/utils.c

INCLUDES = -I../ -I../../deps/$(LIBRESSL)/include \
           -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include 

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)
           
SILENCED_WARNINGS = -Wno-padded \
                    -Wno-unused-parameter \
                    -Wno-pedantic

FLAGS = -DFREEBSD -shared -fno-strict-aliasing -fPIC -Wall -Werror -Weverything -g -O3 -std=c99

LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../crypto/libnxcrypto.so

LIB_NAME = libnxutils.so

endif

ifeq ($(OPERATING_SYSTEM), Linux)
           
SILENCED_WARNINGS = -Wno-padded \
                    -Wno-unused-parameter \
                    -Wno-pedantic
FLAGS += -shared -fPIC
LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../crypto/libnxcrypto.so

LIB_NAME = libnxutils.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SILENCED_WARNINGS = -Wno-unknown-warning-option \
                    -Wno-reserved-id-macro -Wno-padded \
                    -Wno-import-preprocessor-directive-pedantic \
                    -Wno-unused-parameter -Wno-unused-parameter \
                    -Wno-pedantic -Wno-deprecated-declarations

SOURCES += $(ROOT_DIR)/src/utils/SetProcessName.c

FLAGS = -dynamiclib -DMAC_OSX -fno-strict-aliasing -Wall -Werror -Weverything -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../crypto/libnxcrypto.dylib -framework CoreFoundation \
      -framework Carbon

LIB_NAME = libnxutils.dylib

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM

.PHONY: format
format:

	cd $(ROOT_DIR) && clang-format -i -style=file $(SOURCES)

SOURCES = $(ROOT_DIR)/src/memory/memory.c

INCLUDES = -I../ -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SILENCED_WARNINGS = -Wno-padded -Wno-cast-qual \
                    -Wno-used-but-marked-unused

FLAGS = -DFREEBSD -shared -fno-strict-aliasing -fPIC -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99
LIB = ../io/libnxio.so
LIB_NAME = libnxmemory.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SILENCED_WARNINGS += -Wno-cast-qual
FLAGS = -dynamiclib -DMAC_OSX -fno-strict-aliasing -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99
LIB = ../io/libnxio.dylib
LIB_NAME = libnxmemory.dylib

endif

ifeq ($(OPERATING_SYSTEM), Linux)

FLAGS = -DLINUX -DCOMMON -shared -fno-strict-aliasing -fPIC -Wall -Werror -Wextra -Wpedantic -g -O3 -std=gnu99
LIB = ../io/libnxio.so
LIB_NAME = libnxmemory.so

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM

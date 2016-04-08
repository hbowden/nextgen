SOURCES = $(ROOT_DIR)/src/plugins/plugin.c

INCLUDES = -I../ -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SILENCED_WARNINGS = -Wno-padded \
                    -Wno-unused-parameter -Wno-cast-qual 

FLAGS = -DFREEBSD -shared -fno-strict-aliasing -fPIC -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../utils/libnxutils.so
      
LIB_NAME = libnxplugin.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)
           
SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-unused-parameter -Wno-cast-qual 

FLAGS = -dynamiclib -DMAC_OSX -fno-strict-aliasing -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../utils/libnxutils.dylib
      
LIB_NAME = libnxplugin.dylib

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

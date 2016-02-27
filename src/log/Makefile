SOURCES = log.c

INCLUDES = -I../ -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)
           
SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-unused-parameter -Wno-unused-parameter \
                    -Wno-unused-variable

FLAGS = -DFREEBSD -shared -fPIC -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.so ../memory/libnxmemory.so
      
SOURCES += log-freebsd.c

LIB_NAME = libnxlog.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)
           
SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-unused-parameter -Wno-unused-parameter \
                    -Wno-unused-variable

FLAGS = -dynamiclib -DMAC_OSX -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib

SOURCES += log-mac.c
      
LIB_NAME = libnxlog.dylib

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
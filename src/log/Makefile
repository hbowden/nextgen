SOURCES = $(ROOT_DIR)/src/log/log.c

INCLUDES = -I../ -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include \
           -I ../../deps/sqlite \
           -isystem ../../deps/sqlite

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)
           
SILENCED_WARNINGS = -Wno-padded \
                    -Wno-unused-parameter -Wno-unused-parameter \
                    -Wno-unused-variable \
                    -Wno-pedantic

FLAGS = -DFREEBSD -shared -fno-strict-aliasing -fPIC -Wall -Werror -Weverything -g -O3 -std=c99

LIB = ../io/libnxio.so ../memory/libnxmemory.so
      
SOURCES += $(ROOT_DIR)/src/log/log-freebsd.c

INCLUDES += -I/usr/src/cddl/compat/opensolaris/include \
            -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common \
            -I/usr/src/sys/cddl/compat/opensolaris \
            -I/usr/src/sys/cddl/contrib/opensolaris/uts/common \
            -isystem /usr/src/cddl/compat/opensolaris/include \
            -isystem /usr/src/cddl/compat/opensolaris/lib/libdtrace/common \
            -isystem /usr/src/sys/cddl/compat/opensolaris \
            -isystem /usr/src/sys/cddl/contrib/opensolaris/uts/common 

LIB_NAME = libnxlog.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)
           
SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-unused-parameter -Wno-unused-parameter \
                    -Wno-unused-variable

FLAGS = -dynamiclib -DMAC_OSX -fno-strict-aliasing -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib $(ROOT_DIR)/deps/sqlite/.libs/libsqlite3.0.dylib

SOURCES += $(ROOT_DIR)/src/log/log-mac.c
      
LIB_NAME = libnxlog.dylib

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
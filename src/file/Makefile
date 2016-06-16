SOURCES = $(ROOT_DIR)/src/file/file.c

INCLUDES = -I../ -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SILENCED_WARNINGS = -Wno-padded -Wno-cast-qual \
                    -Wno-unused-parameter -Wno-unknown-pragmas \
                    -Wno-pedantic -Wno-sign-conversion \
                    -Wno-used-but-marked-unused

INCLUDES += -I/usr/src/cddl/compat/opensolaris/include \
            -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common \
            -I/usr/src/sys/cddl/compat/opensolaris \
            -I/usr/src/sys/cddl/contrib/opensolaris/uts/common

FLAGS = -DFREEBSD -shared -fno-strict-aliasing -fPIC -Wall -Werror -Weverything -Wpedantic -g -O3

LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../utils/libnxutils.so ../concurrent/libnxconcurrent.so \
      ../mutate/libnxmutate.so ../log/libnxlog.so \
      ../crypto/libnxcrypto.so 

LIB_NAME = libnxfile.so

SOURCES += file-freebsd.c

endif

ifeq ($(OPERATING_SYSTEM), Linux)

FLAGS = -DLINUX -DCOMMON -shared -fno-strict-aliasing -fPIC -Wall -Werror -Wextra -Wpedantic -g -O3

LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../utils/libnxutils.so ../concurrent/libnxconcurrent.so \
      ../mutate/libnxmutate.so ../log/libnxlog.so \
      ../crypto/libnxcrypto.so 

LIB_NAME = libnxfile.so

SOURCES += file-linux.c

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SILENCED_WARNINGS = -Wno-unknown-warning-option \
                    -Wno-reserved-id-macro \
                    -Wno-cast-qual \
                    -Wno-unused-parameter \
                    -Wno-used-but-marked-unused

FLAGS = -dynamiclib -DMAC_OSX -fno-strict-aliasing -Wall -Werror -Weverything -Wpedantic -g -O3

LIB = -lobjc ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../utils/libnxutils.dylib ../concurrent/libnxconcurrent.dylib \
      ../mutate/libnxmutate.dylib ../log/libnxlog.dylib \
      ../crypto/libnxcrypto.dylib ../objc/libnxobjcutils.dylib \
      -framework Foundation -framework AppKit

LIB_NAME = libnxfile.dylib

SOURCES += $(ROOT_DIR)/src/file/file-mac.m

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
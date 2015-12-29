
SOURCES = runtime.c nextgen.c

INCLUDES = -I../ -I../../deps/ck-0.5.0/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded -Wno-cast-qual \
                    -Wno-unused-parameter -Wno-pedantic \
                    -Wno-unknown-pragmas

INCLUDES += -I/usr/src/cddl/compat/opensolaris/include \
            -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common \
            -I/usr/src/sys/cddl/compat/opensolaris \
            -I/usr/src/sys/cddl/contrib/opensolaris/uts/common

FLAGS = -DFREEBSD -shared -fPIC -Wall -Werror -Weverything -Wpedantic -g -O3

LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../utils/libnxutils.so ../concurrent/libnxconcurrent.so \
      ../mutate/libnxmutate.so ../log/libnxlog.so \
      ../crypto/libnxcrypto.so ../syscall/libnxsyscall.so \
      ../disas/libnxdisas.so ../probe/libnxprobe.so \
      ../genetic/libnxgenetic.so ../plugins/libnxplugin.so \
      ../reaper/libnxreaper.so ../resource/libnxresource.so \
      ../file/libnxfile.so

LIB_NAME = libnxruntime.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded -Wno-cast-qual \
                    -Wno-unused-parameter

FLAGS = -dynamiclib -DMAC_OSX -Wall -Werror -Weverything -Wpedantic -g -O3

LIB = -lobjc ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../utils/libnxutils.dylib ../concurrent/libnxconcurrent.dylib \
      ../mutate/libnxmutate.dylib ../log/libnxlog.dylib \
      ../crypto/libnxcrypto.dylib ../syscall/libnxsyscall.dylib \
      ../disas/libnxdisas.dylib ../probe/libnxprobe.dylib \
      ../genetic/libnxgenetic.dylib ../plugins/libnxplugin.dylib \
      ../reaper/libnxreaper.dylib ../resource/libnxresource.dylib \
      ../file/libnxfile.dylib

LIB_NAME = libnxruntime.dylib

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
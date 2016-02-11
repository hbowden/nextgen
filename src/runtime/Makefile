
SOURCES = runtime.c nextgen.c

INCLUDES = -I../ -I../../deps/ck-0.5.0/include

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded -Wno-cast-qual \
                    -Wno-unused-parameter

FLAGS = -Wall -Werror -Weverything -Wpedantic -g -O3

LIB = -lobjc ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../utils/libnxutils.dylib ../concurrent/libnxconcurrent.dylib \
      ../mutate/libnxmutate.dylib ../log/libnxlog.dylib \
      ../crypto/libnxcrypto.dylib ../syscall/libnxsyscall.dylib \
      ../disas/libnxdisas.dylib ../probe/libnxprobe.dylib \
      ../genetic/libnxgenetic.dylib ../plugins/libnxplugin.dylib \
      ../reaper/libnxreaper.dylib ../resource/libnxresource.dylib \
      ../file/libnxfile.dylib

LIB_NAME = libnxruntime.dylib

all:

	$(CC) -dynamiclib -DMAC_OSX $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
SOURCES = test_log.c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src 

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib \
      $(ROOT_DIR)/src/crypto/libnxcrypto.dylib \
      $(ROOT_DIR)/src/syscall/libnxsyscall.dylib \
      $(ROOT_DIR)/src/concurrent/libnxconcurrent.dylib \
      $(ROOT_DIR)/src/resource/libnxresource.dylib \
      $(ROOT_DIR)/src/probe/libnxprobe.dylib \
      $(ROOT_DIR)/src/mutate/libnxmutate.dylib \
      $(ROOT_DIR)/src/log/libnxlog.dylib

FLAGS += -Wno-unused-parameter

all:

	$(CC) $(FLAGS) $(PLATFORM) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o test_log

.PHONY: clean
clean:

	rm -rf test_log
	rm -rf *.dSYM
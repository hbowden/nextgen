SOURCES = $(UNIT_TEST).c ../test_utils.c 

INCLUDES = -I../ -I$(ROOT_DIR)/src

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib \
      $(ROOT_DIR)/src/concurrent/libnxconcurrent.dylib \
      $(ROOT_DIR)/src/crypto/libnxcrypto.dylib \
      $(ROOT_DIR)/src/syscall/libnxsyscall.dylib \
       $(ROOT_DIR)/src/resource/libnxresource.dylib \
      $(ROOT_DIR)/src/log/libnxlog.dylib \
      $(ROOT_DIR)/src/probe/libnxprobe.dylib \
      $(ROOT_DIR)/src/mutate/libnxmutate.dylib

UNIT_TEST = test_generate

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(PLATFORM) $(SOURCES) $(LIB) -o $(UNIT_TEST) 

.PHONY: clean
clean:

	rm -rf $(UNIT_TEST)
	rm -rf *.dSYM
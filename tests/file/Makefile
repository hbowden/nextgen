SOURCES = test_file.c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib \
      $(ROOT_DIR)/src/concurrent/libnxconcurrent.dylib \
      $(ROOT_DIR)/src/crypto/libnxcrypto.dylib \
      $(ROOT_DIR)/src/file/libnxfile.dylib \
      $(ROOT_DIR)/src/log/libnxlog.dylib \
      $(ROOT_DIR)/src/mutate/libnxmutate.dylib

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(PLATFORM) $(LIB) -o test_file

.PHONY: clean
clean:

	rm -rf test_syscall
	rm -rf *.dSYM
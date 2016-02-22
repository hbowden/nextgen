SOURCES = test_reaper.c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib \
      $(ROOT_DIR)/src/concurrent/libnxconcurrent.dylib \
      $(ROOT_DIR)/src/crypto/libnxcrypto.dylib \
      $(ROOT_DIR)/src/syscall/libnxsyscall.dylib \
      $(ROOT_DIR)/src/reaper/libnxreaper.dylib

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(PLATFORM) $(LIB) -o test_reaper

.PHONY: clean
clean:

	rm -rf test_reaper
	rm -rf *.dSYM
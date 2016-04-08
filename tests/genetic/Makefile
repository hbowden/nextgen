SOURCES = test_genetic.c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src \
           -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib \
      $(ROOT_DIR)/src/concurrent/libnxconcurrent.dylib \
      $(ROOT_DIR)/src/crypto/libnxcrypto.dylib \
      $(ROOT_DIR)/src/syscall/libnxsyscall.dylib

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(PLATFORM) $(SOURCES) $(LIB) -o test_genetic

.PHONY: clean
clean:

	rm -rf test_genetic
	rm -rf *.dSYM
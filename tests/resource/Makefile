SOURCES = test_resource.c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src \
           -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib \
      $(ROOT_DIR)/src/concurrent/libnxconcurrent.dylib \
      $(ROOT_DIR)/src/crypto/libnxcrypto.dylib \
      $(ROOT_DIR)/src/syscall/libnxsyscall.dylib \
      $(ROOT_DIR)/src/network/libnxnetwork.dylib
all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o test_resource

.PHONY: clean
clean:

	rm -rf test_resource
	rm -rf *.dSYM
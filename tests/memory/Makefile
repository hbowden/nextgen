SOURCES = test_memory.c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib \
      $(ROOT_DIR)/src/concurrent/libnxconcurrent.dylib

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o test_memory

.PHONY: clean
clean:

	rm -rf test_memory
	rm -rf *.dSYM
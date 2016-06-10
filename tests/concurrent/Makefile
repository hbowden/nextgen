SOURCES = $(UNIT_TEST).c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src \
           -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

LIB = $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/concurrent/libnxconcurrent.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib

UNIT_TEST = test_concurrent

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(PLATFORM) $(INCLUDES) $(SOURCES) $(LIB) -o $(UNIT_TEST) 

.PHONY: clean
clean:

	rm -rf $(UNIT_TEST)
	rm -rf *.dSYM
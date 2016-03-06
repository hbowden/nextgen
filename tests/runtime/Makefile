UNIT_TEST = test_runtime

SOURCES = $(UNIT_TEST).c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src

SILENCED_WARNINGS += -Wno-unused-parameter -Wno-cast-qual

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib \
      $(ROOT_DIR)/src/runtime/libnxruntime.dylib

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(UNIT_TEST)

.PHONY: clean
clean:

	rm -rf $(UNIT_TEST)
	rm -rf *.dSYM
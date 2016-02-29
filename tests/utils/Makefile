UNIT_TEST = test_utils

SOURCES = $(UNIT_TEST).c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/crypto/libnxcrypto.dylib

SILENCED_WARNINGS += -Wno-unused-parameter

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(UNIT_TEST)

.PHONY: clean
clean:

	rm -rf $(UNIT_TEST)
	rm -rf *.dSYM
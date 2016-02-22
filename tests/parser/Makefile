SOURCES = test_cmd_parser.c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib \
      $(ROOT_DIR)/src/concurrent/libnxconcurrent.dylib \
      $(ROOT_DIR)/src/crypto/libnxcrypto.dylib \
      $(ROOT_DIR)/src/runtime/libnxruntime.dylib

all:

	$(CC) $(PLATFORM) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o test_parser

.PHONY: clean
clean:

	rm -rf test_parser
	rm -rf *.dSYM
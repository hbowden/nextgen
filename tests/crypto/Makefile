SOURCES = test_crypto.c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src \
           -I$(ROOT_DIR)/deps/$(LIBRESSL)/include \
           -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib \
      $(ROOT_DIR)/deps/$(LIBRESSL)/crypto/.libs/libcrypto.a

all:

	$(CC) $(FLAGS) $(PLATFORM) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o test_crypto

.PHONY: clean
clean:

	rm -rf test_crypto
	rm -rf *.dSYM
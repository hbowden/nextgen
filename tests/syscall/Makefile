SOURCES = test_syscall.c ../test_utils.c

INCLUDES = -I../ -I$(ROOT_DIR)/src \
           -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

LIB = $(ROOT_DIR)/src/io/libnxio.dylib \
      $(ROOT_DIR)/src/memory/libnxmemory.dylib \
      $(ROOT_DIR)/src/utils/libnxutils.dylib \
      $(ROOT_DIR)/src/concurrent/libnxconcurrent.dylib \
      $(ROOT_DIR)/src/crypto/libnxcrypto.dylib \
      $(ROOT_DIR)/src/syscall/libnxsyscall.dylib \
      $(ROOT_DIR)/src/resource/libnxresource.dylib \
      $(ROOT_DIR)/src/log/libnxlog.dylib \
      $(ROOT_DIR)/src/probe/libnxprobe.dylib \
      $(ROOT_DIR)/src/mutate/libnxmutate.dylib

SILENCED_WARNINGS += -Wno-unused-parameter

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(PLATFORM) $(INCLUDES) $(SOURCES) $(LIB) -o test_syscall

.PHONY: clean
clean:

	rm -rf test_syscall
	rm -rf *.dSYM
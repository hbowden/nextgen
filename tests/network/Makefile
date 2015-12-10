ROOT_DIR = $(shell pwd)

CK = ck-0.5.0

LIBRESSL = libressl-2.3.1

CFLAGS = -Werror -Weverything -Wall -O3 -g

IGNORE = -Wno-reserved-id-macro -Wno-used-but-marked-unused -Wno-format-nonliteral -Wno-padded -Wno-unused-variable \
         -Wno-unused-parameter -Wno-missing-noreturn -Wno-cast-qual -Wno-unused-value

INCLUDES = -I../ -I../../src -I../../deps/$(CK)/include -I$(ROOT_DIR)/../../deps/$(LIBRESSL)/include/

SOURCES = $(UNIT_TEST).c ../test_utils.c ../../src/memory.c ../../src/io.c  \
          ../../src/utils.c ../../src/concurrent.c ../../src/crypto.c

LIBS = $(ROOT_DIR)/../../deps/$(LIBRESSL)/crypto/.libs/libcrypto.a

OPERATING_SYSTEM = $(shell uname)

UNIT_TEST = test_network

ifeq ($(OPERATING_SYSTEM), FreeBSD)

CC = clang

endif

ifeq ($(OPERATING_SYSTEM), Darwin)

CC = clang

OS = -DMAC_OSX

OS_FLAGS = -I../../src/syscalls/mac/

endif

all:

	$(CC) $(CFLAGS) $(OS_FLAGS) $(IGNORE) $(INCLUDES) $(SOURCES) $(LIBS) $(OS) -o $(UNIT_TEST) 

.PHONY: clean
clean:

	rm -rf $(UNIT_TEST)
	rm -rf *.dSYM
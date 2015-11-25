CURRENT_DIR = $(shell pwd)

CK = ck-0.5.0

LIBRESSL = libressl-2.3.1

CFLAGS = -Werror -Weverything -Wall -O3 -g

IGNORE = -Wno-reserved-id-macro -Wno-used-but-marked-unused -Wno-format-nonliteral -Wno-padded

INCLUDES = -I../ -I../../src -I../../deps/$(CK)/include -I$(CURRENT_DIR)/../../deps/$(LIBRESSL)/include/

SOURCES = test_crypto.c ../test_utils.c ../../src/memory.c ../../src/io.c ../../src/crypto.c ../../src/utils.c

LIBS = $(CURRENT_DIR)/../../deps/$(LIBRESSL)/crypto/.libs/libcrypto.a

OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), FreeBSD)

CC = clang

endif

ifeq ($(OPERATING_SYSTEM), Darwin)

CC = clang

OS = -DMAC_OSX

endif

all:

	$(CC) $(CFLAGS) $(IGNORE) $(INCLUDES) $(SOURCES) $(LIBS) $(OS) -o test_crypto

.PHONY: clean
clean:

	rm -rf test_crypto
	rm -rf *.dSYM
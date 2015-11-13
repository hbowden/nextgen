CURRENT_DIR = $(shell pwd)

CFLAGS = -Werror -Weverything -Wall -O3 -g

IGNORE = -Wno-reserved-id-macro -Wno-used-but-marked-unused -Wno-format-nonliteral -Wno-padded -Wno-unused-parameter

INCLUDES = -I../ -I../../src -I../../deps/ck-0.4.5/include -I$(CURRENT_DIR)/../../deps/libressl-2.3.0/include/

SOURCES = test_cmd_parser.c ../test_utils.c ../../src/memory.c ../../src/io.c ../../src/nextgen.c ../../src/utils.c ../../src/crypto.c

LIBS = $(CURRENT_DIR)/../../deps/libressl-2.3.0/crypto/.libs/libcrypto.a

OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), FreeBSD)

CC = clang

endif

ifeq ($(OPERATING_SYSTEM), Darwin)

CC = clang

OS = -DMAC_OSX

endif

all:

	$(CC) $(CFLAGS) $(IGNORE) $(INCLUDES) $(SOURCES) $(LIBS) $(OS) -o test_parser

.PHONY: clean
clean:

	rm -rf test_parser
	rm -rf *.dSYM
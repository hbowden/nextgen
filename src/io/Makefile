
SOURCES = io.c

INCLUDES = -I../

SILENCED_WARNINGS = -Wno-reserved-id-macro \
                    -Wno-format-nonliteral

FLAGS = -dynamiclib -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB_NAME = libnxio.dylib

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
	rm -rf *.dylib
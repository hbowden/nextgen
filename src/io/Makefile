
SOURCES = io.c

INCLUDES = -I../

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SILENCED_WARNINGS = -Wno-reserved-id-macro \
                    -Wno-format-nonliteral

FLAGS = -shared -fPIC -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB_NAME = libnxio.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SILENCED_WARNINGS = -Wno-reserved-id-macro \
                    -Wno-format-nonliteral

FLAGS = -dynamiclib -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB_NAME = libnxio.dylib

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
	rm -rf *.dylib
SILENCED_WARNINGS = -Wno-unknown-warning-option \
                    -Wno-reserved-id-macro \
                    -Wno-padded \
                    -Wno-objc-interface-ivars \
                    -Wno-direct-ivar-access \
                    -Wno-unused-parameter
SOURCES = objc-utils.m AppDelegate.m

INCLUDES = -I../ -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

FLAGS = -DMAC_OSX -dynamiclib -fno-strict-aliasing -fstack-protector-all -Wall -Werror -Weverything -Wpedantic -g -O3

LIB = -framework Foundation -framework AppKit

LIB_NAME = libnxobjcutils.dylib

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM
SOURCES = $(ROOT_DIR)/src/concurrent/concurrent.c

INCLUDES = -I../ -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SOURCES += $(ROOT_DIR)/src/concurrent/concurrent-common.c

SILENCED_WARNINGS += -Wno-format-nonliteral -Wno-gnu-statement-expression -Wno-undef \
                     -Wno-used-but-marked-unused

FLAGS = -DCOMMON -shared -fno-strict-aliasing -fstack-protector-all -fPIC -Wall -Werror -Weverything -g -O3 -std=c99
LIB = ../../deps/$(CK)/src/libck.so ../io/libnxio.so
LIB_NAME = libnxconcurrent.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SOURCES += $(ROOT_DIR)/src/concurrent/concurrent-common.c
SILENCED_WARNINGS += -Wno-format-nonliteral \
                     -Wno-gnu-statement-expression \
                     -Wno-undef

FLAGS = -DCOMMON -dynamiclib -fno-strict-aliasing -fstack-protector-all -Wall -Werror -Weverything -g -O3 -std=c99
LIB = ../../deps/$(CK)/src/libck.so \
      ../io/libnxio.dylib \
      ../memory/libnxmemory.dylib
LIB_NAME = libnxconcurrent.dylib

endif

ifeq ($(OPERATING_SYSTEM), Linux)

SOURCES += $(ROOT_DIR)/src/concurrent/concurrent-common.c
FLAGS += -shared -fPIC -fstack-protector-all
LIB = ../../deps/$(CK)/src/libck.so ../io/libnxio.so
LIB_NAME = libnxconcurrent.so

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM

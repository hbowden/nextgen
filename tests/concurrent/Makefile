INCLUDES = -I../deps/Unity-master/src/ -I../src \
           -I../deps/$(CK)/include \
           -isystem ../deps/$(CK)/include
LIB = -lncurses 
FLAGS += -Wno-switch-enum
TEST_DEPS =  cd $(ROOT_DIR)/deps/$(LIBRESSL) && $(MAKE) check && \
	         cd $(ROOT_DIR)/deps/$(CK) && $(MAKE) check

OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), Darwin)

LIB += ../src/io/libnxio.dylib

else

LIB += ../src/io/libnxio.so

endif

build:
	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(LIB) unit/tests.c $(ROOT_DIR)/deps/Unity-master/src/unity.c -o unit-tests


INCLUDES = -Ideps/Unity-master/src/

build:
	$(CC) $(SILENCED_WARNINGS) $(INCLUDES) tests.c $(ROOT_DIR)/deps/Unity-master/src/unity.c -o test_suite

test:
	./test_suite
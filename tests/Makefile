
INCLUDES = -Ideps/Unity-master/src/

TEST_DEPS =  cd $(ROOT_DIR)/deps/$(LIBRESSL) && $(MAKE) check && \
	         cd $(ROOT_DIR)/deps/$(CK) && $(MAKE) check && \
	         cd $(ROOT_DIR)/deps/$(CAPSTONE) && $(MAKE) check


build:
	$(CC) $(SILENCED_WARNINGS) $(INCLUDES) tests.c $(ROOT_DIR)/deps/Unity-master/src/unity.c -o test_suite

test:
	./test_suite

test-all:
	$(TEST_DEPS)
	./test_suite

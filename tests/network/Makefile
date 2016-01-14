SOURCES = $(UNIT_TEST).c ../test_utils.c ../../src/memory.c ../../src/io.c  \
          ../../src/utils.c ../../src/concurrent.c ../../src/crypto.c

UNIT_TEST = test_network

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(UNIT_TEST) 

.PHONY: clean
clean:

	rm -rf $(UNIT_TEST)
	rm -rf *.dSYM
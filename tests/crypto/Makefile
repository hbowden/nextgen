SOURCES = test_crypto.c ../test_utils.c ../../src/memory.c ../../src/io.c ../../src/utils.c

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o test_crypto

.PHONY: clean
clean:

	rm -rf test_crypto
	rm -rf *.dSYM
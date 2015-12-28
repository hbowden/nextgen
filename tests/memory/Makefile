SOURCES = test_memory.c ../test_utils.c ../../src/memory.c ../../src/io.c ../../src/crypto.c ../../src/utils.c \
          ../../src/nextgen.c
all:

	$(CC) $(CFLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIBS) -o test_memory

.PHONY: clean
clean:

	rm -rf test_memory
	rm -rf *.dSYM
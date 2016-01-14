
UNIT_TEST = test_utils

SOURCES = $(UNIT_TEST).c ../test_utils.c ../../src/memory.c ../../src/io.c \
                         ../../src/crypto.c

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(UNIT_TEST)

.PHONY: clean
clean:

	rm -rf $(UNIT_TEST)
	rm -rf *.dSYM
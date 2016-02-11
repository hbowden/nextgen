SOURCES = test_cmd_parser.c ../test_utils.c ../../src/memory.c ../../src/io.c \
                            ../../src/nextgen.c ../../src/utils.c ../../src/crypto.c
all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o test_parser

.PHONY: clean
clean:

	rm -rf test_parser
	rm -rf *.dSYM
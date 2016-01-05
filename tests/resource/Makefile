SOURCES = test_resource.c ../test_utils.c ../../src/memory.c ../../src/io.c ../../src/nextgen.c ../../src/utils.c ../../src/crypto.c \
          ../../src/network.c ../../src/concurrent.c

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIBS) -o test_resource

.PHONY: clean
clean:

	rm -rf test_resource
	rm -rf *.dSYM
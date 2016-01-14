SOURCES = $(UNIT_TEST).c ../test_utils.c ../../src/generate.c ../../src/network.c ../../src/resource.c \
                         ../../src/concurrent.c ../../src/memory.c ../../src/io.c ../../src/utils.c \
                         ../../src/crypto.c ../../src/arg_types.c ../../src/log.c ../../src/nextgen.c \
                         ../../src/mutate.c ../../src/signals.c ../../src/plugin.c ../../src/shim.c \
                         ../../src/file.c ../../src/genetic.c ../../src/probe.c

UNIT_TEST = test_generate

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(ENTRY_SOURCES) $(INCLUDES) $(SOURCES) $(LIB) -o $(UNIT_TEST) 

.PHONY: clean
clean:

	rm -rf $(UNIT_TEST)
	rm -rf *.dSYM
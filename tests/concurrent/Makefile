SOURCES = $(UNIT_TEST).c ../test_utils.c ../../src/memory.c ../../src/io.c  \
          ../../src/arg_types.c ../../src/utils.c ../../src/shim.c ../../src/nextgen.c ../../src/crypto.c \
          ../../src/signals.c ../../src/log.c ../../src/concurrent.c ../../src/mutate.c ../../src/plugin.c \
          ../../src/genetic.c ../../src/syscall.c ../../src/generate.c ../../src/network.c ../../src/resource.c \
          ../../src/file.c

UNIT_TEST = test_concurrent

all:

	$(CC) $(CFLAGS) $(SILENCED_WARNINGS) $(ENTRY_SOURCES) $(INCLUDES) $(SOURCES) $(LIBS) -o $(UNIT_TEST) 

.PHONY: clean
clean:

	rm -rf $(UNIT_TEST)
	rm -rf *.dSYM
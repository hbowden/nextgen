SOURCES = test_syscall.c ../test_utils.c ../../src/memory.c ../../src/io.c  \
          ../../src/arg_types.c ../../src/utils.c ../../src/shim.c ../../src/nextgen.c ../../src/crypto.c \
          ../../src/signals.c ../../src/log.c ../../src/concurrent.c ../../src/mutate.c ../../src/plugin.c \
          ../../src/genetic.c ../../src/generate.c ../../src/network.c ../../src/resource.c ../../src/file.c \
          ../../src/probe.c

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(ENTRY_SOURCES) $(INCLUDES) $(SOURCES) $(LIB) -o test_syscall

.PHONY: clean
clean:

	rm -rf test_syscall
	rm -rf *.dSYM
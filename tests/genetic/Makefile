SOURCES = test_genetic.c ../test_utils.c ../../src/memory.c ../../src/io.c  \
          ../../src/arg_types.c ../../src/utils.c ../../src/nextgen.c ../../src/crypto.c \
          ../../src/signals.c ../../src/log.c ../../src/concurrent.c ../../src/mutate.c ../../src/plugin.c \
          ../../src/syscall/syscall.c ../../src/generate.c ../../src/network.c ../../src/resource.c ../../src/file.c \
          ../../src/probe/probe.c

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(ENTRY_SOURCES) $(OS_SOURCES) $(LIB) -o test_genetic

.PHONY: clean
clean:

	rm -rf test_genetic
	rm -rf *.dSYM
SOURCES = test_reaper.c ../test_utils.c ../../src/memory.c ../../src/io.c ../../src/reaper.c ../../src/syscall.c \
          ../../src/arg_types.c ../../src/utils.c ../../src/shim.c ../../src/nextgen.c ../../src/crypto.c \
          ../../src/signals.c ../../src/log.c ../../src/concurrent.c ../../src/mutate.c ../../src/plugin.c \
          ../../src/genetic.c ../../src/generate.c ../../src/network.c ../../src/resource.c ../../src/file.c

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(ENTRY_SOURCES) $(INCLUDES) $(SOURCES) $(LIBS) -o test_reaper

.PHONY: clean
clean:

	rm -rf test_reaper
	rm -rf *.dSYM
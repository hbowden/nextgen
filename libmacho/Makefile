
CC = clang

SOURCES = command.c macho.c section.c segment.c symbol.c symtab.c

OBJECTS = command.o macho.o section.o segment.o symbol.o symtab.o

CFLAGS = -Wall -Werror -Weverything -g -c -fstack-protector-all -O3 -Wno-documentation -Wno-deprecated-declarations -Wno-newline-eof -Wno-padded

LIBRARY = libmacho.a

.PHONY: all
all: $(SOURCES) $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	ar rc $(LIBRARY) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:

	rm *.o
	rm $(LIBRARY)
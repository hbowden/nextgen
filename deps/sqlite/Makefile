
OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), Darwin)

COMMAND = cc -dynamiclib -fPIC -O3 -o libsqlite3.0.dylib sqlite3.c

endif

ifeq ($(OPERATING_SYSTEM), Linux)

COMMAND = cc -shared -ldl -lpthread -fPIC -O3 -o libsqlite3.so sqlite3.c

endif

ifeq ($(OPERATING_SYSTEM), FreeBSD)

COMMAND = cc -shared -fPIC -O3 -o libsqlite3.so sqlite3.c

endif

ifeq ($(shell uname -o), Cygwin)

COMMAND = cc -shared -O3 -o libsqlite3.dll sqlite3.c

endif

all:
	$(COMMAND)

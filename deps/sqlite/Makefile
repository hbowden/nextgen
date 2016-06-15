
OPERATING_SYSTEM = $(shell uname)

ifeq ($(OPERATING_SYSTEM), Darwin)

COMMAND = cc -dynamiclib -fPIC -O3 -o libsqlite3.0.dylib sqlite3.c

ifeq ($(OPERATING_SYSTEM), Linux)

COMMAND = cc -shared -ldl -fPIC -O3 -o sqlite3.so sqlite3.c

else

COMMAND = cc -shared -fPIC -O3 -o sqlite3.so sqlite3.c

endif

all:
	$(COMMAND)
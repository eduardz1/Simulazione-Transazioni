CC=gcc
CFLAGS=-std=c89 -g -O0 -pedantic -D_GNU_SOURCE

# Dependencies
SHARED=src/common.c src/include/common.h src/utils/debug.c src/utils/debug.h
MASTER=src/master.c src/include/master.h src/print.c src/include/print.h src/parser.c src/include/parser.h $(SHARED)
USER=src/users.c src/include/users.h $(SHARED)
NODE=src/nodes.c src/include/nodes.h $(SHARED)

all: master users nodes

master: $(MASTER)
	$(CC) $(CFLAGS) src/master.c src/common.c src/print.c src/parser.c src/utils/debug.c -lm -o master

users: $(USER)
	$(CC) $(CFLAGS) src/users.c src/common.c src/utils/debug.c -lm -o users

nodes: $(NODE)
	$(CC) $(CFLAGS) src/nodes.c src/common.c src/utils/debug.c -lm -o nodes

clean:
	rm -f *.o master users nodes *~

run:
	./master
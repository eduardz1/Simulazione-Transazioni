CC=gcc
CFLAGS=-std=c89 -g -O0 -pedantic -D_GNU_SOURCE

all: master users nodes

master: src/master.c src/include/master.h src/common.c src/include/common.h
	$(CC) $(CFLAGS) src/master.c src/common.c src/print.c src/parser.c -lm -o master

users: src/users.c src/include/users.h src/common.c src/include/common.h
	$(CC) $(CFLAGS) src/users.c src/common.c -lm -o users

nodes: src/nodes.c src/common.c src/include/common.h
	$(CC) $(CFLAGS) src/nodes.c src/common.c -lm -o nodes

clean:
	rm -f *.o master users nodes *~

run:
	./master
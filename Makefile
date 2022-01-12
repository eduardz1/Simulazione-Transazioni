CC=gcc
CFLAGS=-std=c89 -g -O0 -pedantic -D_GNU_SOURCE
DEBUG=-DDEBUG

# Dependencies
SHARED=src/common.c src/include/common.h src/utils/*.h src/utils/*.c
MASTER=src/master.c src/include/master.h src/print.c src/include/print.h src/parser.c src/include/parser.h
USER=src/users.c src/include/users.h
NODE=src/nodes.c src/include/nodes.h

all: master users nodes
	rm -f *.o *~

master: $(MASTER) $(SHARED)
	make shared 
	$(CC) $(CFLAGS) src/master.c  src/print.c src/parser.c *.o -lm -o master

users: $(USER) $(SHARED)
	make shared
	$(CC) $(CFLAGS) src/users.c *.o -lm -o users

nodes: $(NODE) $(SHARED)
	make shared
	$(CC) $(CFLAGS) src/nodes.c *.o -lm -o nodes

shared: $(SHARED)
	$(CC) -c $(CFLAGS) src/common.c src/utils/*.c

debug:
	rm -f *.o master users nodes *~
	$(CC) -c $(CFLAGS) $(DEBUG) src/common.c src/utils/*.c
	$(CC) $(CFLAGS) $(DEBUG) src/master.c  src/print.c src/parser.c *.o -lm -o master
	$(CC) $(CFLAGS) $(DEBUG) src/users.c *.o -lm -o users
	$(CC) $(CFLAGS) $(DEBUG) src/nodes.c *.o -lm -o nodes
	./master

clean:
	rm -f *.o master users nodes *~

run: all
	./master
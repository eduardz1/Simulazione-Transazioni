CC=gcc
CFLAGS=-std=c89 -pedantic -D_GNU_SOURCE
DEBUG=-DDEBUG -g -Wall -O0

# Dependencies
SHARED=src/include/common.h src/utils/*.h src/utils/*.c
OBJS=lklist.o msg.o pool.o debug.o sem.o
MASTER=src/master.c src/include/master.h src/print.c src/include/print.h src/parser.c src/include/parser.h
USER=src/users.c src/include/users.h
NODE=src/nodes.c src/include/nodes.h

all: master users nodes

master: $(OBJS) $(MASTER)
	$(CC) $(CFLAGS) -O2 src/master.c  src/print.c src/parser.c *.o -lm -o master

users: $(OBJS) $(USER)
	$(CC) $(CFLAGS) -O2 src/users.c src/print.c *.o -lm -o users

nodes: $(OBJS) $(NODE)
	$(CC) $(CFLAGS) -O0 src/nodes.c src/print.c *.o -lm -o nodes

%.o: src/utils/%.c $(SHARED)
	$(CC) -c $(CFLAGS) -O2 src/utils/*.c

debug:
	rm -f *.o master users nodes *~
	$(CC) -c $(CFLAGS) $(DEBUG) src/utils/*.c
	$(CC) $(CFLAGS) $(DEBUG) src/master.c  src/print.c src/parser.c *.o -lm -o master
	$(CC) $(CFLAGS) $(DEBUG) src/users.c src/print.c *.o -lm -o users
	$(CC) $(CFLAGS) $(DEBUG) src/nodes.c src/print.c *.o -lm -o nodes

clean:
	rm -f *.o master users nodes log.txt ledger.txt *~

run: all
	./master
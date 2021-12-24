CC=gcc
CFLAGS=-std=c89 -g -O0 -pedantic -D _GNU_SOURCE

include conf.txt

all: master transactions users nodes

master: src/master.c # master.o
	$(CC) $(CFLAGS) master.c $(SO_USER_NUM) $(SO_NODES_NUM) $(SO_NUM_FRIENDS) $(SO_SIM_SEC) -o master 

transactions: src/transaction.c src/include/transaction.h # transactions.O
	$(CC) $(CFLAGS) transaction.c $(SO_HOPS) -o transaction

users: src/users.c src/include/users.h # users.o
	$(CC) $(CFLAGS) users.c $(SO_BUDGET_INIT) $(SO_REWARD) $(SO_MIN_TRANS_GEN_NSEC) $(SO_MAX_TRANS_GEN_NSEC) $(SO_RETRY) -o users

nodes: src/nodes.c # nodes.o
	$(CC) $(CFLAGS) nodes.c $(SO_TP_SIZE) $(SO_BLOCK_SIZE) -o nodes

clean:
	rm -f *.o master transactions users nodes *~

run: all
	./a
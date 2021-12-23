CC=gcc
CFLAGS=-std=c89 -g -O0 -pedantic -D _GNU_SOURCE

# Master
SO_USER_NUM=5
SO_NODES_NUM=5
SO_NUM_FRIENDS=2
SO_SIM_SEC=120

# Transactions
SO_HOPS=3

# Users
SO_BUDGET_INIT=100
SO_REWARD=5
SO_MIN_TRANS_GEN_NSEC=200
SO_MAX_TRANS_GEN_NSEC=400
SO_RETRY=5

# Nodes
SO_TP_SIZE=20
SO_BLOCK_SIZE=5

all: master transactions users nodes

master: src/master.c # master.o
	$(CC) $(CFLAGS) master.c $(SO_USER_NUM) $(SO_NODES_NUM) $(SO_NUM_FRIENDS) $(SO_SIM_SEC) -o master 

transactions: src/transaction.c # transactions.O
	$(CC) $(CFLAGS) transaction.c $(SO_HOPS) -o transaction

users: src/users.c # users.o
	$(CC) $(CFLAGS) users.c $(SO_BUDGET_INIT) $(SO_REWARD) $(SO_MIN_TRANS_GEN_NSEC) $(SO_MAX_TRANS_GEN_NSEC) $(SO_RETRY) -o users

nodes: src/nodes.c # nodes.o
	$(CC) $(CFLAGS) nodes.c $(SO_TP_SIZE) $(SO_BLOCK_SIZE) -o nodes

clean:
	rm -f *.o master transactions users nodes *~

run: all
	./a
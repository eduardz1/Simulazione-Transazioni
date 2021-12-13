CC=gcc
CFLAGS=-std=89 -g -O0 -pedantic

# Master
SO_USER_NUM=10
SO_NODES_NUM=2
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

all: master trans users nodes
	 

master: Master.c Master.o
	$(CC) $(CFLAGS) Master.c $(SO_USER_NUM) $(SO_NODES_NUM) $(SO_NUM_FRIENDS) $(SO_SIM_SEC) -o Master 

trans: Transactions.c Transactions.O
	$(CC) $(CFLAGS) Transactions.c $(SO_HOPS) -o Transactions

users: Users.c Users.o
	$(CC) $(CFLAGS) Users.c $(SO_BUDGET_INIT) $(SO_REWARD) $(SO_MIN_TRANS_GEN_NSEC) $(SO_MAX_TRANS_GEN_NSEC) $(SO_RETRY) -o Users

nodes: Nodes.c Nodes.o
	$(CC) $(CFLAGS) Nodes.c $(SO_TP_SIZE) $(SO_BLOCK_SIZE) -o Nodes

CC=gcc
CFLAGS=-std=c89 -g -O0 -pedantic -D_GNU_SOURCE

include conf.txt

# lines, in order, indicate arguments of main, user and node
CL_ARGUMENTS_ALL=$(SO_USER_NUM) $(SO_NODES_NUM) $(SO_NUM_FRIENDS) $(SO_SIM_SEC) \
				 $(SO_BUDGET_INIT) $(SO_REWARD) $(SO_RETRY) $(SO_MIN_TRANS_GEN_NSEC) $(SO_MAX_TRANS_GEN_NSEC) \
				 $(SO_TP_SIZE) $(SO_BLOCK_SIZE)

all: master users nodes

master: src/master.c # master.o
	$(CC) $(CFLAGS) master.c $(CL_ARGUMENTS_ALL) -o master 

#transactions: src/transaction.c src/include/transaction.h # transactions.O
#	$(CC) $(CFLAGS) transaction.c $(SO_HOPS) -o transaction

users: src/users.c src/include/users.h # users.o
	$(CC) $(CFLAGS) users.c -o users

nodes: src/nodes.c # nodes.o
	$(CC) $(CFLAGS) nodes.c -o nodes

clean:
	rm -f *.o master users nodes *~

run: all
	./a
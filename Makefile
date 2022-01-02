CC=gcc
CFLAGS=-std=c89 -g -O0 -pedantic -D_GNU_SOURCE

include conf.txt

# lines, in order, indicate arguments of main, user and node
CL_ARGUMENTS_ALL=$(SO_USER_NUM) $(SO_NODES_NUM) $(SO_NUM_FRIENDS) $(SO_SIM_SEC) \
				 $(SO_BUDGET_INIT) $(SO_REWARD) $(SO_RETRY) $(SO_MIN_TRANS_GEN_NSEC) $(SO_MAX_TRANS_GEN_NSEC) \
				 $(SO_TP_SIZE) $(SO_BLOCK_SIZE)

all: master users nodes

master: src/master.c src/include/master.h src/common.c src/include/common.h
	$(CC) $(CFLAGS) src/master.c src/common.c src/print.c -lm -o master

users: src/users.c src/include/users.h src/common.c src/include/common.h
	$(CC) $(CFLAGS) src/users.c src/common.c -lm -o users

nodes: src/nodes.c src/common.c src/include/common.h
	$(CC) $(CFLAGS) src/nodes.c src/common.c -lm -o nodes

clean:
	rm -f *.o master users nodes *~

run: 
	./master $(CL_ARGUMENTS_ALL)
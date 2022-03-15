CC=gcc
CFLAGS= -std=c89 -pedantic -D_GNU_SOURCE -lpthread
DEBUG= -DDEBUG -g -Wall -O0

# Dependencies
MASTER=source/Master.c source/include/Master.h
USERS=source/Users.c source/include/Users.h
NODES=source/Nodes.c source/include/Nodes.h
PRINT=source/Print.c source/include/Print.h

all: Master  Users Nodes

master:$(MASTER)
	$(CC) $(CFLAGS) -O2 source/Master.c -o Master

users: $(USER)
	$(CC) $(CFLAGS) -O2 source/Users.c  -o Users

nodes: $(NODE)
	$(CC) $(CFLAGS) -O0 source/Nodes.c -o Nodes

print:$(PRINT)
	$(CC) $(CFLAGS) -O0 source/Print.c -o Print

run: ./Master

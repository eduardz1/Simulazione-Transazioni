CC=gcc
CFLAGS= -std=c89 -g  -Wall  -pedantic -D_GNU_SOURCE 


# Dependencies
MASTER=source/Master.c source/include/Master.h
USERS=source/Users.c source/include/Users.h
NODES=source/Nodes.c source/include/Nodes.h
PRINT=source/Print.c source/include/Print.h
PARSER=source/Parser.c source/include/Parser.h

conf:
	$(CC) $(CFLAGS) source/Conf.c	

delConf:
	source ./delConf.sh

all:nodes master users 

master:$(MASTER)
	$(CC) $(CFLAGS)  source/Master.c -o Master

user: $(USERS)
	$(CC) $(CFLAGS)  source/Users.c  -o Users

nodes: $(NODE)
	$(CC) $(CFLAGS)  source/Nodes.c -o Nodes

print:$(PRINT)
	$(CC) $(CFLAGS)  source/Print.c  -v -lpthread -o Print
parser:$(PARSER)	
	$(CC) $(CFLAGS)  source/Parser.c -o Parser
	
run: ./Master

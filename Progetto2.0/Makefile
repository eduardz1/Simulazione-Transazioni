CC=gcc
CFLAGS= -std=c89 -g  -Wall  -pedantic -D_GNU_SOURCE 


# Dependencies
MASTER=source/Master.c source/include/Master.h
USERS=source/Users.c source/include/Users.h
NODES=source/Nodes.c source/include/Nodes.h
PRINT=source/Print.c source/include/Print.h
PARSER=source/Parser.c source/include/Parser.h

all:nodes master users print

master:$(MASTER)
	$(CC) $(CFLAGS)  source/Master.c source/Util/IPCS_SEM.c source/Util/Ms_Queue.c -o Master

users: $(USERS)
	$(CC) $(CFLAGS)  source/Users.c source/Util/IPCS_SEM.c source/Util/Ms_Queue.c -o Users

nodes: $(NODE)
	$(CC) $(CFLAGS)  source/Nodes.c source/Util/IPCS_SEM.c source/Util/Ms_Queue.c -o Nodes

print:$(PRINT)
	$(CC) $(CFLAGS)  source/Print.c  -v -lpthread -o Print
	
run:
	./Master

clean:
	rm Master Nodes Users Print 

ipc_clean:
	ipcrm -a
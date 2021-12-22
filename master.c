/* #define _GNU_SOURCE  need to define before including headers, I'm defining it into the Makefile */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define SO_USERS_NUM
#define SO_NODES_NUM
#define SO_NUM_FRIENDS
#define SO_SIM_SEC
int main(){
	/* 
	 * get every user_pid, ask to libro_mastro 
	 * to return it's current_budget and print
	 * repeat every second until simulation persists
	 * remember that CTRL-C should kill the simulation
	 */

	/* 
	 * need to define a kill signal for the simulation, either:
	 * - SO_SIM_SEC seconds have passed
	 * - libro_mastro is full
	 * - CTRL-C from stdin
	 */

	/* 
	 * end of simulation should print:
	 * - kill signal
	 * - balance of every user, as before, may need to write a funciton for that
	 * - balance of every node (function as before but with different parameter)
	 * - number of user processes aborted
	 * - number of blocks in the libro_mastro
	 * - number of transaction still in the pool, for each node
	 */
	
	int Prop1 = fork(SO_NODES_NUM);
	int Prop2 = fork(SO_USERS_NUM);
	switch (fork()){
		case -1: /* Case Error */
        		if (Prop1 && Prop2 == 0)
        		{
            			printf("Fork error");
            			exit(1);
        		}
        		break;
    		case 0: /* Case Child */
        		if (Prop1 && Prop2 > 0)
        		{
            			/* printf("Child pid of SO_NODES_NUM is:%d\n", getpid(),Prop1); */
            			/* printf("Child pid of SO_USERS_NUM is%d\n", getpid(),Prop2); */
        		}

        	break;
		
		default: /* Case Parent */
        		fork(SO_NODES_NUM);
          
        		fork(SO_USERS_NUM);
        		break;
    	}
}

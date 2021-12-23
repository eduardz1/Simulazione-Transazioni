#ifndef SIMULAZIONE_TRANSAZIONI_MASTER_H
#define SIMULAZIONE_TRANSAZIONI_MASTER_H

/*
 * probabilmente tutte set macro non ci servono perche' ha più senso passare i
 * valori come parametri in compilazione
 */

/* libro mastro ----> linked list */

/*
 * execve(const char *user, char *const argv[], char *const envp[])
 * in user_fork to link "user" executable to the forked process, same thing for nodes
 * check lesson on pipes by prof. bini, the second hour
 */

/*
 * every transaction should be noted and we need to manage inconsistency with
 * semaphores maybe
 */

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

#include "clock.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#define SO_REGISTRY_SIZE     /* max length of consecutive block */
#define SO_BLOCK_SIZE        /* number of transaction per block*/
/*
 * #define SO_USERS_NUM
 * #define SO_NODES_NUM
 */
#define SO_NUM_FRIENDS

typedef struct node {
    timestamp timeStamp;
    pid_t sender;
    pid_t receiver;
    int quantity;
    int reward;

    struct node *next_node;
};

typedef struct block {
    SO_NODES_NUM;

    struct node **block;
    struct block **next_block;
};

void interrupt_handle(int SIGINT);

#endif /* SIMULAZIONE_TRANSAZIONI_MASTER_H */

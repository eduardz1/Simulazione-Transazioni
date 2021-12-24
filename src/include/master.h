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

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#define SO_BLOCK_SIZE 100 /* number of transaction per block*/
#define SO_REGISTRY_SIZE 1000 /* max length of consecutive blocks */


/* Transaction struct */
typedef struct transaction
{
    struct timespec timestamp;
    pid_t sender;
    pid_t receiver;
    int amount;
    /* int reward; */
} transaction;

/* Block struct */
typedef struct block
{
    struct transaction transList[SO_BLOCK_SIZE];
    unsigned int blockIndex; /* when a block is written on ledger it's Index needs to be updated */
    struct block *next;
} block;

/* Libro Mastro (ledger) struc */
typedef struct ledger
{
    struct block *head;
    unsigned int registryCurrSize; /* initialize to SO_REGISTRY_SIZE, update with every new block added */
} ledger;



void interrupt_handle(int signum);
pid_t spawn_user();
pid_t sapwn_node();

#endif /* SIMULAZIONE_TRANSAZIONI_MASTER_H */

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

#include <string.h>
#include <signal.h>
/*Prototype*/
void make_arguments(int *IPCarray, char *argv[]);

void spawn_user(char *argv[], int counter);
void spawn_node(char *argv[], int counter);

void shared_memory_objects_init(int *shared_memory_objects_IDs);
void semaphores_init();
void message_queue_init();
void make_ipc_array(int *IPC_objects_IDs);

void master_interrupt_handle(int signum);

void message_queue_init();
void spawn_user(char *userArgv[], int uCounter);
void spawn_node(char *nodeArgv[], int nCounter); 
void shared_memory_objects_init(int *shmArray); 
void semaphores_init(); 
void make_ipc_array(int *IPC_array);
void start_continuous_print(); 
void master_interrupt_handle(int signum); 



#endif /* SIMULAZIONE_TRANSAZIONI_MASTER_H */

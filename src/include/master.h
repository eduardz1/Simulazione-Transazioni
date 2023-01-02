#ifndef SIMULAZIONE_TRANSAZIONI_MASTER_H
#define SIMULAZIONE_TRANSAZIONI_MASTER_H

#include <malloc.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>

void make_arguments(int *IPCarray, char *argv[]);

/* fork and exec a "./users" */
void spawn_user(char *argv[], size_t counter);

/* fork and exec a "./nodes" */
int spawn_node(char *argv[], size_t counter);
void make_friend_list(pid_t *friends);

void shd_memory_init(void);
void semaphores_init(void);

/* init message queue to receive transactions that have hopped too much, queue
 * is initialized with master's pid as key */
int message_queue_init(void);

void make_ipc_array(int *IPC_objects_IDs);
void send_friends(pid_t node);

void master_interrupt_handle(int signum);

void handle_extra_nodes(char **argvSpawns, pid_t *privateFriends);

#endif /* SIMULAZIONE_TRANSAZIONI_MASTER_H */

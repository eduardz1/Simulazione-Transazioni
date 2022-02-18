#ifndef SIMULAZIONE_TRANSAZIONI_MASTER_H
#define SIMULAZIONE_TRANSAZIONI_MASTER_H

#include "../utils/msg.h"

#include <string.h>
#include <signal.h>

void make_arguments(int *IPCarray, char *argv[]);

void spawn_user(char *argv[], int counter);
int spawn_node(char *argv[], int counter);
void make_friend_list(pid_t *friends);

void shared_memory_objects_init(int *shared_memory_objects_IDs);
void semaphores_init();
int message_queue_init();
void make_ipc_array(int *IPC_objects_IDs);
void make_friend_list(pid_t *friends);
void send_friends(pid_t node);

void master_interrupt_handle(int signum);

#endif /* SIMULAZIONE_TRANSAZIONI_MASTER_H */

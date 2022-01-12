#ifndef SIMULAZIONE_TRANSAZIONI_USERS_H
#define SIMULAZIONE_TRANSAZIONI_USERS_H

#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

void user_transactions_handle(int signum);
void user_interrupt_handle(int signum);

int get_pid_userIndex(int PID_toSearch);
pid_t get_random_userPID(user *usersPID);
pid_t get_random_nodePID(node *nodesPID);

#endif /* SIMULAZIONE_TRANSAZIONI_USERS_H */

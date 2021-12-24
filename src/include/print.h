#ifndef SIMULAZIONE_TRANSAZIONI_PRINT_H
#define SIMULAZIONE_TRANSAZIONI_PRINT_H

#include <stdio.h>
#include <sys/types.h>

/* function that prints on terminal the PID of every user and node process */
void print_user_nodes_table(pid_t main, pid_t *user, pid_t *nodes, int userNum, int nodesNum);

#endif /* SIMULAZIONE_TRANSAZIONI_PRINT_H */
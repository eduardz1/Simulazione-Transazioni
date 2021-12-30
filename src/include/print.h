#ifndef SIMULAZIONE_TRANSAZIONI_PRINT_H
#define SIMULAZIONE_TRANSAZIONI_PRINT_H

#include <stdio.h>
#include <sys/types.h>


void print_user_nodes_table(pid_t main, pid_t *user, pid_t *nodes, int userNum, int nodesNum); /* function that prints on terminal the PID of every user and node process */
void print_kill_signal(); /* need to define, prints reason of termination (simTime elapsed/ledger full/every process terminated) */
void print_user_balance(); /* need to define, prints balance of every user */
void print_node_balance(); /* need to define, prints balance of every node */
void print_num_aborted(); /* need to define, prints num of processes aborted */
void print_num_blocks(); /* need to define, prints num of blocks saved in the ledger */
void print_transactions_still_in_pool(); /* need to define, prints num of transactions still in the pool of each node */

#endif /* SIMULAZIONE_TRANSAZIONI_PRINT_H */
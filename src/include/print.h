#ifndef SIMULAZIONE_TRANSAZIONI_PRINT_H
#define SIMULAZIONE_TRANSAZIONI_PRINT_H

#include "common.h"
#include "../utils/pool.h"

void print_time_to_die();
void print_user_nodes_table(pid_t main, user *user, node *nodes, struct parameters *par); /* function that prints on terminal the PID of every user and node process */
void print_kill_signal();                                                                 /* need to define, prints reason of termination (simTime elapsed/ledger full/every process terminated) */
void print_user_balance();                                                                /* need to define, prints balance of every user */
void print_node_balance();                                                                /* need to define, prints balance of every node */
void print_num_aborted();                                                                 /* need to define, prints num of processes aborted */
void print_num_blocks();                                                                  /* need to define, prints num of blocks saved in the ledger */
void print_transactions_still_in_pool();                                                  /* need to define, prints num of transactions still in the pool of each node */

void final_print(pid_t masterPID, user *usersPID, node *nodesPID, struct parameters *par);
void print_parameters(struct parameters *par);

/* formatting ledger and blocks */
void print_block(block *b);
void print_transaction(transaction *t);
void print_ledger(block *l);

void formatted_timestamp(FILE *fp);
void print_transaction_pool(pool *transPool);

#endif /* SIMULAZIONE_TRANSAZIONI_PRINT_H */
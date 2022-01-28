#ifndef SIMULAZIONE_TRANSAZIONI_PRINT_H
#define SIMULAZIONE_TRANSAZIONI_PRINT_H

#include "common.h"
#include "../utils/pool.h"
#include "../utils/lklist.h"
#include "../utils/msg.h"

void print_user_nodes_table(pid_t main, user *user, node *nodes, struct parameters *par); /* function that prints on terminal the PID of every user and node process */
void print_kill_signal();                                                                 /* need to define, prints reason of termination (simTime elapsed/ledger full/every process terminated) */                                                              /* need to define, prints balance of every node */
void print_num_aborted(user *usersPID, struct parameters *par);                                                                 /* need to define, prints num of processes aborted */
void print_num_blocks(block *l);                                                                  /* need to define, prints num of blocks saved in the ledger */

void final_print(pid_t masterPID, user *usersPID, node *nodesPID, struct parameters *par, block *ledger);
void print_most_significant_processes(user *userPID, node *nodePID, struct parameters *par);
void print_parameters(struct parameters *par);

/* formatting ledger and blocks */
void print_block(block *b, FILE *fp);
void print_transaction(transaction *t, FILE *fp);
void print_ledger(block *l);

void formatted_timestamp(char *, struct timespec);
void print_transaction_pool(pool *transPool);
void print_outgoing_pool(struct node *outPool);
void print_current_budget(struct parameters *par,struct user_t *usr,struct node_t *node);
#endif /* SIMULAZIONE_TRANSAZIONI_PRINT_H */
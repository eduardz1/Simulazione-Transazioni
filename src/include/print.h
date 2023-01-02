#ifndef SIMULAZIONE_TRANSAZIONI_PRINT_H
#define SIMULAZIONE_TRANSAZIONI_PRINT_H

#include "../utils/arraylist.h"
#include "../utils/msg.h"
#include "common.h"
#include <bits/types/FILE.h>

enum Term
{
    TIME_ELAPSED,
    USER_DEAD,
    LEDGER_FULL
};

void print_user_nodes_table(pid_t main,
                            User *user,
                            Node *nodes,
                            struct Params *par);
void print_kill_signal(enum Term);
void print_num_aborted(User *usersPID, struct Params *par);
int print_num_blocks(Ledger *l);

void print_most_significant_processes(User *userPID,
                                      Node *nodePID,
                                      struct Params *par);
void print_parameters(struct Params *par);

/* formatting ledger and blocks */
void print_block(Block *b, FILE *fp, size_t index);
void print_transaction(Transaction *t, FILE *fp);
void print_ledger(Ledger *l);

void formatted_timestamp(char *, struct timespec);

#endif /* SIMULAZIONE_TRANSAZIONI_PRINT_H */

#ifndef SIMULAZIONE_TRANSAZIONI_LKLIST_H
#define SIMULAZIONE_TRANSAZIONI_LKLIST_H

#include "../include/common.h"

struct node
{
    transaction trans;
    struct node *next;
};

int compare_transactions(transaction *t1, transaction *t2);
struct node *new_node(transaction t);
void push(struct node *head, transaction t);
void find_and_remove(struct node *head, transaction *toSearch);

#endif /* SIMULAZIONE_TRANSAZIONI_LKLIST_H */
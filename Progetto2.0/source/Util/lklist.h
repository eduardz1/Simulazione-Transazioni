#include "../include/Common.h"

struct node *new_node(transaction t);
void push(struct node *head, transaction t);
int compare_transaction(transaction *t1, transaction *t2);
void find_and_remove(struct node **head, transaction *search);
#include "../include/Common.h"



struct Node{
     
    transaction trans; 
    int Data ;
    struct Node *next ; 
};

int compare_transactions(transaction *t1, transaction *t2);
struct Node *new_node(transaction t);
void push(struct Node *head, transaction t);
void find_and_remove(struct Node **head, transaction *toSearch);
#include "lklist.h"

int compare_transactions(transaction *t1, transaction *t2)
{
    if (t1->timestamp.tv_nsec == t2->timestamp.tv_nsec &&
        t1->timestamp.tv_sec == t2->timestamp.tv_sec &&
        t1->amount == t2->amount &&
        t1->receiver == t2->receiver)
    {
        return 1;
    }
    return 0;
}

/* allocates and returns a new node */
struct node *new_node(transaction t)
{
    struct node *newNode = malloc(sizeof(struct node)); /* might be a memory leak but I have no idea about the way I should dinamically deallocate it */
    if (newNode == NULL){
        TRACE(("*** malloc failed in %s:%d, system out of memory ***\n", __FILE__, __LINE__))
    }

    newNode->trans = t;
    newNode->next = NULL;

    return newNode;
}

/* pushes transaction 't' at the end of the list */
void push(struct node *head, transaction t)
{
    struct node *curr = head;
    struct node *newNode;
    while (curr->next != NULL)
    {
        curr = curr->next;
    }

    newNode = new_node(t);
    curr->next = newNode;
}

/* finds and removes a message from pool if present */
void find_and_remove(struct node **head, transaction *toSearch)
{
    struct node *curr = *head;
    struct node *prev = NULL;

    if (*head == NULL)
        return;

    while (!compare_transactions(&curr->trans, toSearch))
    {
        if (curr->next == NULL)
        {
            return; /* transaction not found */
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
    }

    if (curr == *head)
    {
        *head = (*head)->next;
    }
    else
    {
        prev->next = curr->next;
    }

    /* free(prev); */
}
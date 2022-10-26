#include "lklist.h"

struct node *new_node(transaction t)
{
 struct node *newNode = malloc(sizeof(struct node));
  if (newNode == NULL)
  {
    printf("Error allocating memory for new node\n");
  }
   newNode->trans=t;  /*TODO why is this not working? */
  newNode->next = NULL;
  return newNode;
}


/*set transaction at the end of the linked list*/
void push(struct node *head, transaction t)
{
  struct node *curr = head;
  while (curr->next != NULL)
  {
    curr = curr->next;
  }
  curr->next = new_node(t);
}

int compare_transaction(transaction *t1, transaction *t2)
{
  if (t1->Money == t2->Money && t1->Receiver == t2->Receiver && t1->time.tv_nsec == t2->time.tv_sec && t1->time.tv_nsec && t2->time.tv_nsec)
  {
    return 1;
  }
  return 0;
}

/* find and removes a message from pool if present */
void find_and_remove(struct node **head, transaction *search)
{
  struct node *curr = *head;
  struct node *prev = NULL;

  if (*head == NULL)
  {
    return;
  }
  while (!compare_transaction(&curr->trans, search))
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
    free(*head);
    *head = (*head)->next;
  }
  else
  {

    prev->next = curr->next;
    free(curr);
  }
}


/* FUNZIONI PER LA LINKED LIST ,UTILIZZATA PER LA TRANS POOL*/



#include "tnslist.h"




int compare_transaction(transaction *t1 , transaction*t2){
    if(t1->Money == t2->Money)
    return 1;
    else
    return 0 ; 
}


struct Node * size_list(transaction t ){ 
    
    struct Node *node= (struct Node*) malloc(sizeof(struct Node)*SO_BLOCK_SIZE*2); 
  /*    struct Node *next =(struct Node *)malloc(sizeof(struct Node)*SO_BLOCK_SIZE*2);  */
  return node; 
}

void push_to_linked(struct Node *Node ,transaction t){
    
    Node->Data=t.Money+t.Receiver+t.Reward+t.Sender;
    Node->next=size_list(t);

}

void pop (struct Node **head , transaction *to_search){
    struct Node *curr = *head; 
    struct Node *prev =NULL; 
    if (*head == NULL)
    {
        printf("HEAD == NULL \n"); 
        return; 
    }

    else
    {
        prev = curr;
        curr=curr->next;
    }
    
    if (curr == *head)
    {
        free(*head); 
        *head=(*head)->next;
    }
    
}
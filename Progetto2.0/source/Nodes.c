#include "include/Nodes.h"
#include "/Users/popper/Desktop/Folders/RiProgetto/Simulazione-Transazioni/Progetto2.0/Util/Ms_Queue.h"
#include <stdlib.h>
/*#define SENDER -1*/
/*struct mesg_buffer *MessageQ;*/
pool transPool;
struct msgbuf_trans *newTransaction;
struct msgbuf_trans *Trans_ptr ; 
#define PROC_PERMISSION 0666 /* CHMOD value for root permission */ 

int MessagId;
int Message_Erro; 
int Money_q; 
int toend ; 
pid_t myPID; 

    //DA RIFARE MSQUEUE CREARE HEADER FILE APPOSITO E SVOLGERE DA  0 

/*https://www.geeksforgeeks.org/ipc-using-message-queues*/ 
int sum_reward(transaction *sumBlock)
{
    int i = 0;
    int sum = 0;

    for (i = 0; i < (SO_BLOCK_SIZE - 1); i++)
    {
        sum += sumBlock[i].Reward;
    }
    return sum ;
}

/*transaction pool is a linked list that contains struct of message with all information (see Common header) 
 * to understand free and malloc look up to composite_data_type structure last slide 
 * */
void transaction_pool_init(pool *transPool){
  transPool->head=NULL;
  transPool->tail=NULL;
  transPool->size=0;
}

int add_to_pool(pool *transPool, struct msgbuf_trans *message){
  struct msgbuf_trans *newTransaction = malloc(sizeof(struct msgbuf_trans));
    if(newTransaction==NULL){
      printf("----malloc error-----");
    }

  newTransaction->Message_Transaction=message->Message_Transaction;
  newTransaction->Message_Transaction.next=NULL;

  if(transPool->tail){
    transPool->head=newTransaction;

  }
  return SUCCESS; 
}

int remove_from_pool(pool *transPool, struct msgbuf_trans *message_t){
  if(transPool==NULL){
    return ERROR;
  } else if(transPool->head==NULL){
    return ERROR;
  }
  *message_t= *transPool->head;
  free(transPool->head);
  transPool->head=transPool->head->Message_Transaction.next;

 if(transPool->head==NULL){
    transPool->tail=NULL;
  }
  return SUCCESS;
}


int main(){
  Message_queue();
  transaction_pool_init(&transPool);   
  add_to_pool(&transPool,newTransaction);
  remove_from_pool(&transPool, newTransaction);
}


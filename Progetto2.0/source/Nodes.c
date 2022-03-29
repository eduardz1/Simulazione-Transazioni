#include "include/Nodes.h"
#include <stdlib.h>
/*#define SENDER -1*/
/*struct mesg_buffer *MessageQ;*/
pool transPool;
struct msgbuf_trans *newTransaction;

/*
int MessagId;

void Reward(){
int Money_q; 
pid_t myPID; 
    
}
/https://www.geeksforgeeks.org/ipc-using-message-queues/ 
/NON SO DA DOVE DEVO PRENDERE LO STREAM DI DATI PER I NODI  
void Message_queue(){
    key_t KeyQ;  
    int sendMes; 
    KeyQ=ftok("TransaPool",99); 
    MessagId=msgget(KeyQ,0666|IPC_CREAT); 
   if(MessagId==-1){ 
       perror("MSGGET");
        return EXIT_FAILURE; 
   }
   printf("TRANSACTION :"); 
    Message sender 
    msgsnd(MessagId,MessageQ,sizeof(MessageQ),IPC_NOWAIT); if fail return EAGAIN
    printf("---DEBUG TOOL---\n");
    printf("Data Send is : %s \n",MessageQ->mes_type);
    msgrcv( MessagId,MessageQ ,sizeof(MessageQ),MessageQ->mes_type,MSG_NOERROR); remove message from queue  is not specified, then the message isn't removed from the queue and the system call fails returning -1 with errno set to E2BIG.
    printf("---DEBUG TOOL---\n");
    printf("message kill PID: Messagid",MessagId); 
}


void message_queue_attach()
{
    do
    {
        MessagId = msgget(myPID, 0);
    } while (errno == ENOENT);
}
*/


/*transaction pool is a linked list that contains struct of message with all information (see Node header) */
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

  newTransaction->transactionMessage=message->transactionMessage;
  newTransaction->transactionMessage.next=NULL;

  if(transPool->tail){
    transPool->head=newTransaction;

  }
  return SUCCESS; 
}

int remove_from_pool(pool *transPool,struct msgbuf_trans *message_t){
  if(transPool==NULL){
    return ERROR;
  } else if(transPool->head==NULL){
    return ERROR;
  }
  *message_t=*transPool->head;
  free(transPool->head);
  transPool->head=transPool->head->transactionMessage.next;

  if(transPool->head==NULL){
    transPool->tail=NULL;
  }
  return SUCCESS;
}


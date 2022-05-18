#include "include/Nodes.h"
#include "include/Master.h"
/*#define SENDER -1*/
/*struct mesg_buffer *MessageQ;*/
pool transPool;
struct msgbuf_trans *newTransaction;
struct msgbuf_trans *Trans_ptr ; 
#define BUFF_MAX 10 
 int Msg_ID ; 

int Message_Erro; 
int Money_q; 
int toend ; 
pid_t myPID;
key_t MSG_Key;  

    

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
  struct msgbuf_trans *newTransaction = malloc(sizeof(newTransaction));
    if(newTransaction==NULL){
      perror("MALLOC ERROR IN POOL\n ");
      exit(EXIT_FAILURE);
    }
while(newTransaction!=NULL){
  newTransaction->Message_Transaction=message->Message_Transaction;
  newTransaction->Message_Transaction.next=NULL;

  if(transPool->tail){
    transPool->head=newTransaction;

  }
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

void Message_Queue(){ 
   
    nPid = fork(); 
    MSG_Key= &nPid; 
    if (MSG_Key==NULL)
    {
      perror("KEY PROBLEM IN MSG_QUEUE CHECK IT \n ");
      exit(EXIT_FAILURE); 
     }
    
    Msg_ID = msgget(MSG_Key, 0666|IPC_CREAT); 
    newTransaction->m_type=1; 
    printf("WRITE DATA \n "); 
    fgets(Trans_ptr->mesText,BUFF_MAX,stdin);  
    /*MEssage to send */
    msgsnd(Msg_ID,Trans_ptr,sizeof(Trans_ptr->Message_Transaction),0);
    printf("DAta Send :%s \n",Trans_ptr->mesText);
    Message_Rec(); 
}
void Message_Rec(){ 
 MSG_Key= &nPid; 
 Msg_ID= msgget(MSG_Key,0666 |IPC_CREAT);
 msgrcv(Msg_ID,Trans_ptr,sizeof(Trans_ptr->Message_Transaction), 1, 0 );  
 printf("DATA RECIVED : %s \n",Trans_ptr->mesText);
 msgctl(Msg_ID,IPC_RMID,NULL);
}



int main(){
  /*Message_Queue();*/
  transaction_pool_init(&transPool);   
  add_to_pool(&transPool,newTransaction);
  remove_from_pool(&transPool, newTransaction);
}


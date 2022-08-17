#include "include/Nodes.h"
#include "include/Master.h"
#include "include/Conf.h"
/*#define SENDER -1*/
/*struct mesg_buffer *MessageQ;*/
pool transPool;
Message message;
Message *newTransaction;
Message *Trans_ptr ; 
Message fetchMex; 
pid_t * friendList ; 
Block_ * ledger; 
#define BUFF_MAX 10 
/*POINTER TO STRUCTURE  */ 
user * UserID; 
node * NodeID; 
Block_ *Ledger; 

/* -------------*/
int Msg_ID ; 
int Message_Erro; 
int Money_q; 
int toend ; 
pid_t myPID;
key_t MSG_Key;  
int friendList_size; 



void take_transaction(){ 
  unsigned int friendCycle =20 ; 
  friend_msg  friends_recived ; 
 int sizeofFriend = friendList_size; 
  if (transPool.size < SO_TP_SIZE && friendCycle<20 )
  {
    if(receive_message(Msg_ID, &fetchMex , sizeof(Message), message.m_type ,0 )==0) 
    {
    add_to_pool(&transPool, &fetchMex); 
    transPool.size++;
    }
  
  
  if(receive_message(Msg_ID, &friends_recived , sizeof(Message), message.m_type,IPC_NOWAIT )==0) 
  {
  friendList = realloc(friendList, sizeof(pid_t)*(friendList_size+1)); 
  friendList ++ ; 
  friendList[sizeofFriend] = friends_recived.friend;; 
  }
  
friendCycle ++ ; 
}
else{ 
  /* Random friends da implemmentare funzione */ 
    friendCycle = 0 ;

 }
}


void Block(transaction * blockT, Block_ *newBlock ) 
{ 
    int i ; 
    transaction reward;
    struct timespec time; 
    bzero(&time , sizeof(time)); 
    clock_gettime(CLOCK_REALTIME, &time);
    reward.Sender=NO_BLOCK; /* Macro defined in common.h*/
    reward.time = time; 
    reward.Receiver=myPID; 
    reward.Reward=0;
    reward.MoneyStatusTrans=pending; 
    reward.Money = sum_reward(blockT); 
    newBlock->t_list[0] = reward;

    for ( i = 1; i < SO_BLOCK_SIZE; i++)
    {
      newBlock->t_list[i] = blockT[i-1];
    }
    

}
void transListTo_block(transaction * Noreward)
{ 
  int i ; 
  Message tmp ; 
  bzero(&tmp , sizeof(tmp));
  for ( i = 0; i < (SO_BLOCK_SIZE-1); i++)
  {
    if (remove_from_pool(&transPool, &tmp)==PROBLEM)
    {
       printf("Problem in NODE n * %d\n", myPID);
       kill(myPID,SIGINT);
       exit(EXIT_FAILURE); 
    }
    Noreward[i] = tmp.Message_Transaction.uTrans;
    
    transPool.size--; 
  }
   
}

void fill_friends(pid_t * friendList) 
{
  unsigned int i ; 
  friend_msg friendMex; 

  bzero(&friendMex , sizeof(friendMex));/* azzero i byte in memoria */
  for ( i = 0; i < SO_FRIENDS_NUM; i++)
  {
    receive_message(Msg_ID, &friendMex , sizeof(friend_msg), friendMex.mtype,0 );
    friendList[i] = friendMex.friend;
  }
  

}

void confirm_state_block(Block_ * confirmed)
{
  int i ; 
  for ( i = 0; i < SO_BLOCK_SIZE; i++)
  {
    confirmed->t_list[i].MoneyStatusTrans = confirmed;
  }
}


void block_ladger(Block_ * newBlock )
{ 
  int i ; 
  Block_ tmp ; 
  for(i = 0 ; SO_REGISTRY_SIZE; i++ )
  {
    if(ledger[i].t_list[1].Sender==0)
    {
      /* Casto nel blocco tmp un nuovo nodo con tutte le sue caratteristiche */
      tmp = *newBlock ; 
      tmp.blockIndex =1; 
      /*da aggiungere Ttramite semaforo le risorse  */
      ledger[i]=tmp;
      confirm_state_block(&ledger[i]);
      /*inserire funzione per il rilascio delle risorse tramite semaforo */ 
      return;
    }
  }
  printf("NODE n * %d ledger is full i can t insert block \n",myPID);
  killpg(0,SIGINT); /* invia segnali ad un gruppo di process almeno questo ho capito */
}

/*


*/

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

void shm_Attach(char **argv)
{
   UserID = shmat(USERS_PID_ARGV, NULL , 0 ); 
   NodeID = shmat(NODES_PID_ARGV, NULL , 0 );
   Ledger = shmat(LEDGER_ARGV, NULL , 0 );
   /*MAncano i semafori  */
}
void get_pid_index()
{
  unsigned int i ; 
  i =0 ; 
  for ( i = 0; i < SO_NODES_NUM *2 ; i++)
  {
    if (NodeID[i].nodPid==myPID)
    {
      return i;
    }
  }
   return PROBLEM;    
  
}




/*transaction pool is a linked list that contains struct of message with all information (see Common header) 
 * to understand free and malloc look up to composite_data_type structure last slide 
 * */
void transaction_pool_init(pool *transPool){
  transPool->head=NULL;
  transPool->tail=NULL;
  transPool->size=0;
}

int add_to_pool(pool *transPool, Message *message){
    newTransaction = malloc(sizeof(newTransaction));
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

int remove_from_pool(pool *transPool, Message *message_t){
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
    Message_Rec( Msg_ID, myPID); 
}
 void Message_Rec(int messageID ,key_t messageKey ){ 
 MSG_Key= &nPid; 
 Msg_ID= msgget(MSG_Key,0666 |IPC_CREAT);
 msgrcv(Msg_ID,Trans_ptr,sizeof(Trans_ptr->Message_Transaction), 1, 0 );  
 printf("DATA RECIVED : %s \n",Trans_ptr->mesText);
 msgctl(Msg_ID,IPC_RMID,NULL);
}



int main(){
  /*Message_Queue();*/
int i , j ,l ; 
printf("TEST MAIN \n"); 
for(i = 0 ; i < SO_NODES_NUM; i++){ 
      transaction_pool_init(&transPool); 
      Message_Queue(); 
      add_to_pool(&transPool , &message); 
      Message_Rec(Msg_ID, MSG_Key); 
      remove_from_pool(&transPool , &message); 


}





}


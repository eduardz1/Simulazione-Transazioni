#include "include/Users.h"

Message *tns;
node *tmp;
node *sendingTransaction;
user *usersPid;
node *nodesPid;
unsigned int actBalance;
int queueID;
pid_t myPid;
unsigned int currentBalance;
node *outGoingTransactions=NULL;


/*function to handle transaction pool easily(linked list util) */

/*allocates new node */
int send_message(int queueID,void *msg,int size,int flag) {
  if (msgsnd(queueID, msg, size, flag) == 0) {
    return 0; /*SUCCESS*/
  }
  switch (errno) {
    case EACCES:
      fprintf(stderr, "[MSG SEND %d] no write permission on queue\n", getpid());
      break;
    case EAGAIN:
      fprintf(stderr, "[MSG SEND %d] couldn't write on queue\n", getpid());
      break;
    case EFAULT:
      fprintf(stderr, "[MSG SEND %d] address pointed by message inaccessible\n", getpid());
      break;
    case EIDRM:
      fprintf(stderr, "[MSG SEND %d] message queue removed\n", getpid());
      break;
    case EINTR:
      fprintf(stderr, "[MSG SEND %d] signal caught when waiting for queue to free\n", getpid());
      break;
    case EINVAL:
      fprintf(stderr, "[MSG SEND %d] invalid  msqid  value,  or nonpositive mtype value, or invalid msgsz value\n",
              getpid());
      break;
    case ENOMEM:
      fprintf(stderr, "[MSG SEND %d] system out of memory\n", getpid()); /* hoping this never happens */
      break;
    default:
      return 0;
      break;
  }
  return -1;
}


node *new_node(transaction t){
  node *newNode= malloc(sizeof(node));
  if(newNode==NULL){
    printf("Error allocating memory for new node\n");
  }
  /* newNode->transaction=t;  TODO why is this not working? */
  newNode->next=NULL;
  return newNode;
}

/*set transaction at the end of the linked list*/
void push(node *head, transaction t){
  node *curr=head;
  while(curr->next!=NULL){
    curr=curr->next;
  }
  curr->next=new_node(t);
}

int get_pid_userIndex(int searchPid) {
  unsigned int i;

  for (i = 0; i < SO_USERS_NUM; i++) {
    if (usersPid[i].usPid == searchPid)
      return i;
  }
  return -1; /*error*/
}

/* update status of user , NB--> 0 alive, 1 broke, 2 dead*/
void update_status(int setStatus) {
  int i = get_pid_userIndex(myPid);

  if (i == -1) {
    printf("user failed to find");
  }

  usersPid[i].Us_state = setStatus;
  if (setStatus == 2) {
    printf("dead");
  }
}

void start_transaction(int money, int reward) {
  struct timespec exTime = {0};
  clock_gettime(CLOCK_REALTIME, &exTime);

  tns->m_type = TRANSACTION_MTYPE;
  tns->Message_Transaction.uTrans.Sender = myPid;
  tns->Message_Transaction.uTrans.Receiver = usersPid;
  tns->Message_Transaction.uTrans.Money = money;
  tns->Message_Transaction.uTrans.Reward = reward;
  tns->Message_Transaction.uTrans.MoneyStatusTrans = pending;

  tns->Message_Transaction.uTrans.time= exTime;
  tns->Message_Transaction.hops = SO_HOPS;

}

int send_transaction(){
  transaction sent={0};
  if(send_message(queueID,&tns,sizeof(tns),IPC_NOWAIT)==0){
    printf("[USER %d] sent a transaction of %d UC to [USER %d] via queue %d\n", myPid, tns->Message_Transaction.uTrans.Money, tns->Message_Transaction.uTrans.Receiver, queueID);
    currentBalance-=(tns->Message_Transaction.uTrans.Money + tns->Message_Transaction.uTrans.Reward);
    sent=tns->Message_Transaction.uTrans;
    if(outGoingTransactions==NULL) {
      outGoingTransactions = new_node(sent);
    }else {
      push(outGoingTransactions, sent);
    }
    return 0;

  }
  return -1; /*error*/
}


void Sh_MemUser(key_t key,size_t size,int shmflg){
    int Mem_id; 
    int Sh_MemInit=shmget(key,sizeof(SO_USERS_NUM),IPC_CREAT|0666); /*define area*/
    char *shmAttach=shmat(Sh_MemInit,NULL,0); /*Attach Area*/
    int ShDet=shmdt(Sh_MemInit); /*Detach Area*/ 

}

/*saves user balance when the program is interrupted in tmpBalance*/
void CurrentBalance() {
  int i;
  int j;
  int accumulate = 0;
  unsigned int tmpBalance = SO_BUDGET_INIT;
  Block_ *tmpLedger[SO_REGISTRY_SIZE];

  for (i = 0; i < SO_REGISTRY_SIZE;i++) {
    /*if transaction is out-going remove Money+Reward else add to receiver Money
     */
    for (j = 0; j < SO_BLOCK_SIZE; j++) {
      if (tmpLedger[i]->t_list[j].Sender == myPid) {
        accumulate -= (tmpLedger[i]->t_list[j].Money + tmpLedger[i]->t_list[j].Reward);
      } else if (tmpLedger[i]->t_list[j].Receiver == myPid) {
        accumulate += tmpLedger[i]->t_list[j].Money;
      }
    }
  }
  tmp = sendingTransaction;

  while (tmp != NULL) {
    accumulate -= (tmp->transaction->Money + tmp->transaction->Reward);
    tmp = tmp->next;
  }
  if (accumulate * (-1) > tmpBalance) {
    fprintf(stderr,"*** [USER %d] errror in calculating balance, overflow ***\n",myPid);
    update_status(2);
    killpg(0, SIGINT);
  }
}

int main() {
  unsigned int amount,reward,retry,money;
  pid_t UsPid,NdPid;
  myPid=getpid();
   actBalance=SO_BUDGET_INIT;
   myPid=getpid();
  start_transaction(money,reward);
  send_transaction();
  /*signal_handler(SIGINT, SIG_IGN);*/

  return 0;
}
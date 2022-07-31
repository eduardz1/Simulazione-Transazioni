#include "include/Users.h"

struct ConfigParameters *par;
Message *tns;
struct node *tmp;
struct node *sendingTransaction;
pid_t myPid;
user *usersPid;
node *nodesPid;
unsigned int actBalance;
int queueID;
pid_t myPid;
unsigned int currentBalance;
node *outGoingTransactions=NULL;

/*function to handle transaction pool easily(linked list util) */

/*allocates new node */
node *new_node(transaction t){
  node *newNode= malloc(sizeof(node));
  if(newNode==NULL){
    printf("Error allocating memory for new node\n");
  }
  newNode->trans=t;
  newNode->next=NULL;
  return newNode;
}

/*set transaction at the end of the list*/
void push(node *head, transaction t){
  node *curr=head;
  while(curr->next!=NULL){
    curr=curr->next;
  }
  curr->next=new_node(t);
}

int get_pid_userIndex(int searchPid) {
  unsigned int i;

  for (i = 0; i < par->SO_USER_NUM; i++) {
    if (usersPid[i].usPid == searchPid)
      return i;
  }
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
  tns->Message_Transaction.hops = par->SO_HOPS;
}

int send_transaction(){
  transaction sent={0};

  if(send_message(queueID,&tns,sizeof(tns),IPC_NOWAIT)==0){
    printf("[USER %d] sent a transaction of %d UC to [USER %d] via queue %d\n", myPid, tns->Message_Transaction.uTrans.Money, tns->Message_Transaction.uTrans.Receiver, queueID);
    currentBalance-=(tns->Message_Transaction.uTrans.Money+tns->Message_Transaction.uTrans.Reward);
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
    int Sh_MemInit=shmget(key,sizeof(par->SO_USER_NUM),IPC_CREAT|0666); /define area/
    char*shmAttach=shmat(Sh_MemInit,NULL,0); /*Attach Area*/
    int ShDet=shmdt(Sh_MemInit); /*Detach Area*/ 

}

/*saves user balance when the program is interrupted in tmpBalance*/
void CurrentBalance() {
  int i;
  int j;
  int accumulate = 0;
  unsigned int tmpBalance = par->SO_BUDGET_INIT;
  block *tmpLedger[SO_REGISTRY_SIZE];

  for (i = 0; i < SO_REGISTRY_SIZE; < i++) {
    /*if transaction is out-going remove Money+Reward else add to receiver Money
     */
    for (j = 0; j < SO_BLOCK_SIZE; i++) {
      if (tmpLedger[i].t_list[j].Sender == myPid) {
        accumulate -=
            (tmpLedger[i].t_list[j].Money + &tmpLedger[i].t_list[j].Reward);
      } else if (tmpLedger[i].t_list[j].Receiver == myPid) {
        accumulate += tmpLedger[i].t_list[j].Money;
      }
    }
  }
  tmp = sendingTransaction;

  while (tmp != NULL) {
    accumulate -= (tmp->transaction.Money + tmp->transaction.Reward);
    tmp = tmp->next;
  }
  if (accumulate * (-1) > tmpBalance) {
    fprintf(stderr,
            "*** [USER %d] errror in calculating balance, overflow ***\n",myPID);
    update_status(2);
    killpg(0, SIGINT);
  }
}

transaction create_transaction(){
      t
}
int main() {
  unsigned int amount,reward,retry;
  pid_t UsPid,NdPid;
  myPid=getpid();
   actBalance=par->SO_BUDGET_INIT;
  /*signal_handler(SIGINT, SIG_IGN);*/


}
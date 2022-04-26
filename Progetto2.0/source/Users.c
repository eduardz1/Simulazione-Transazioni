#include "include/Common.h"
#include <bits/time.h>
#include <sys/types.h>
#include <time.h>

struct ConfigParameters *par;
struct msgbuf_trans *tm;
struct node *tmp;
struct node *sendingTransaction;
pid_t myPid;
user *usersPid;
node *nodesPid;

int getPidUserIndex(int searchPid) {
  unsigned int i;

  for (i = 0; i < par->SO_USER_NUM; i++) {
    if (usersPid[i].pid == searchPid)
      return i;
  }
}

/* update status of user , NB--> 0 alive, 1 broke, 2 dead*/
void updateStatus(int setStatus) {
  int i = getPidUserIndex(myPid);

  if (i == -1) {
    printf("user failed to find");
  }

  usersPid[i].status = setStatus;
  if (setStatus == 2) {
    printf("dead");
  }
}

void start_transaction(pid_t usersPid, int money, int reward) {
  struct timespec exTime = {0};
  clock_gettime(CLOCK_REALTIME, &exTime);

  tm->m_type = TRANSACTION_MTYPE;
  tm->transactionMessage.uTrans.Sender = myPid;
  tm->transactionMessage.uTrans.Receiver = usersPid;
  tm->transactionMessage.uTrans.Money = money;
  tm->transactionMessage.uTrans.Reward = reward;
  tm->transactionMessage.uTrans.T_status = pending;

  tm->transactionMessage.uTrans.time = exTime;
  tm->transactionMessage.hops = par->SO_HOPS;
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
  block tmpLedger[SO_REGISTRY_SIZE];

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
            "*** [USER %d] errror in calculating balance, overflow ***\n",
            myPID);
    update_status(2);
    killpg(0, SIGINT);
  }
}

int main() { printf("i'm users"); /*just a debug print*/ }

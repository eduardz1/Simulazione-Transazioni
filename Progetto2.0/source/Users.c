#include "include/Common.h"
#include "Util/Ms_Queue.h"
/*#include <bits/time.h>*/
#include <sys/types.h>
#include <time.h>

struct _ConfigParameters *par;
struct msgbuf_trans *tm; 
struct node *tmp;
struct _Block *Block ;
struct node *sendingTransaction;
pid_t myPid;
user *usersPid;
node *nodesPid;

int getPidUserIndex(int searchPid) {
  unsigned int i;

  for (i = 0; i < par->SO_USER_NUM; i++) {
    if (usersPid[i].usPid == searchPid)
      return i;
  }
}

/* update status of user , NB--> 0 alive, 1 broke, 2 dead*/
void updateStatus(int setStatus) {
  int i = getPidUserIndex(myPid);

  if (i == -1) {
    printf("user failed to find");
  }

  usersPid[i].Us_state = setStatus;
  if (setStatus == 2) {
    printf("dead");
  }
}

void start_transaction(pid_t usersPid, int money, int reward) {
  struct timespec exTime = {0};
  clock_gettime(CLOCK_REALTIME, &exTime);

  tm->m_type = TRANSACTION_MTYPE;
  tm->Message_Transaction.uTrans.Sender = myPid;
  tm->Message_Transaction.uTrans.Receiver = usersPid;
  tm->Message_Transaction.uTrans.Money = money;
  tm->Message_Transaction.uTrans.Reward = reward;
  tm->Message_Transaction.uTrans.MoneyStatusTrans = pending;

  /*tm->Message_Transaction.uTrans. = exTime;*/
  tm->Message_Transaction.hops = ConfigParameters.SO_HOPS;
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
  Block tmpLedger[SO_REGISTRY_SIZE];

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

#include "include/Common.h"
#include "Util/Ms_Queue.h"
/*#include <bits/time.h>*/
#include <sys/types.h>
#include <time.h>

configparameter  *par;
struct msgbuf_trans *tm; 
node *tmp;
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
    int Sh_MemInit=shmget(key,sizeof(par->SO_USER_NUM),IPC_CREAT|0666); /*define area*/
    char*shmAttach=shmat(Sh_MemInit,NULL,0); /*Attach Area*/
    int ShDet=shmdt(Sh_MemInit); /*Detach Area*/ 

}

void Kill_User(int signo){
    int i;
    for(i=0;i<par->SO_USER_NUM;i++){
        kill(usersPid[i].usPid,SIGKILL);
    }
    exit(0);
}

void Alive_User(pid_t myPid, int money , user * UserStatus){
    int i;
    for(i=0;i<par->SO_USER_NUM;i++){
        if(UserStatus[i].usPid==myPid){
            UserStatus[i].Us_state=ALIVE;
        }
    }
}

/*saves user balance when the program is interrupted in tmpBalance*/
void CurrentBalance(user * UserBalance , pid_t myPid , transaction * PidSender) {
    int i ; 
    int Use_Balence  = UserBalance->balance; 
    int Pid_Sender = PidSender->Sender;
    
    for ( i = 0; Pid_Sender==myPid ; i++)
    {
      getBalamce(); 
    }
    

}

int main() { 
    int i ; 
    int money;
    int reward;
    user * usersPid;
    transaction * Money; 
    transaction * Reward ; 
    money = Money->Money;
    reward = Reward->Reward;

    int usPid = usersPid->usPid;
  for ( i = 0; i < par->SO_USER_NUM; i++)
  {
    usersPid[i].usPid = fork();
    if (usersPid[i].usPid <= 0){
      exit(EXIT_FAILURE); 
    } 
    else{ 
      start_transaction( usPid , money , reward);
    }
    
  }
  exit(EXIT_SUCCESS);
  
  }

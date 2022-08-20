#include "include/Master.h" 

#include <stdlib.h>
#define USER_NAME "./Users"
#define USER_NODE "./Nodes"


user *usersPid;
node *nodesPid;
Block_ ledger[SO_REGISTRY_SIZE];
Block_ *tmpLedger;
int mQueue;

int message_queue_id(){
	int pidGot=getpid();
	int qId=msgget(pidGot,IPC_CREAT| IPC_EXCL| 0666);

	switch(errno){
		case EIDRM:
        printf("[PROCESS %d] queue %d was removed\n", pidGot, qId);
        break;
    case EINVAL:
        printf("[PROCESS %d] queue %d invalid value for cmd or msqid\n", pidGot, qId);
        break;
    case EPERM:
        printf("[PROCESS %d] queue %d the effective user ID of the calling process is not the creator or the owner\n", pidGot, qId);
        break;
    }
    return qId;
	}

void Sh_MemMaster( key_t key,size_t size,int shmflg){  
     int m_id; 
     int ShInit;
     char  *shm;  
     int shmdet; 
     ShInit=shmget(key,sizeof(SO_REGISTRY_SIZE)*2,IPC_CREAT|0666); /*ShdMem Define Area*/  /* Raddoppio l'area per evitare saturazioni*/
     shm=shmat(ShInit,NULL,0); /*Attach ShMem;*/ 
     shmdet=shmdt(ShInit); /*Detach ShMem*/
    system("./Users");
}


void Sh_UserPID(key_t key,size_t size,int shmflg){ 
 key=getpid(); 
 int Sh_UserPIDDet;
 int Sh_UserPIDInit;
 int id; 
 char *Sh_UserPIDAttach;
 Sh_UserPIDInit=shmget(key,sizeof(SO_USERS_NUM),IPC_CREAT|0666); 
 Sh_UserPIDAttach=shmat(Sh_UserPID,NULL,0); 
 Sh_UserPIDDet=shmdt(Sh_UserPIDInit); 
}


void Shared_Memory( key_t key,size_t size,int shmflg){  
     int m_id; 
     int *shm; 
     int ShInit; 
     ShInit=shmget(key,sizeof(SO_REGISTRY_SIZE)*2,IPC_CREAT|0666); /*ShdMem Define Area*/  
     shm=shmat(ShInit,NULL,0); /*Attach ShMem;*/ 
     int shmdt(const void *shmaddr); /*Detach ShMem*/
}



/* generate the user with fork and lauch ./users with execve*/
void generate_user(){    /*need to implement uCounter !! */
     int j ; 
     int i;
     printf("generateUser\n");
     for ( j = 0; j <SO_USERS_NUM; j++)
     {
     pid_t uPid=fork();
     switch(uPid){
      case -1 : 
          perror("USER SPAWN ERROR CHECK IT \n"); 
          exit(EXIT_FAILURE); 
          break;
      case 0 : 
          User[j].usPid=getpid(); 
          User[j].balance=0; 
          User[j].Us_state=ALIVE; 
      break;
      default: 
      break;  
      } 
     }
     for(i =0;i<20;i++){
     printf(" pid %d",User[i].usPid);
     printf("balance %d",User[i].balance);
     printf(" us state%d\n",User[i].Us_state); /* debug*/
     }
}
void generate_node(){
     int i;
     for ( i = 0; i < SO_NODES_NUM; i++)
     {
     pid_t nPid=fork();
      switch (nPid)
      {
      case -1:
           perror("NODE'S FAILURE");
           exit(EXIT_FAILURE);
           break;
      case 0:
          Node[i].nodPid=getpid();
          Node[i].balance=0;
      break;
      default:
           break;
      }
     }          
 }


/* Stop Simulation handler Ctrl-C */
void signal_handler(int signum){
     char c;
     signal(signum,SIG_IGN);
     printf("you pressed CTRL-C\n" "would you like to quit?[y/n]");
     c=getchar();
     if(c=='y'||c=='Y'){
          exit(0);
     } else {
        /*  signal(SIGINT,signal_handler);*/
          getchar();
     }
}





int main(){
	int i;
	unsigned int uCounter;
	unsigned int nCounter;

	struct sigaction sa;
	
	tmpLedger=ledger;
	

	mQueue=message_quque_init();

	for(nCounter=0;nCounter<SO_NODES_NUM;nCounter++){
		nodesPid[nCounter].Node_state=ALIVE;
		nodesPid[nCounter].balance=0;
		generate_user();
	}

	for(uCounter=0;uCounter<SO_USERS_NUM;uCounter++){
		usersPid[uCounter].User_state=ALIVE;
		usersPid[uCounter].balance=0;
		generate_user();
	}

	alarm(SO_SIM_SEC);

	switch (fork())
	{
	case -1:
		fprintf(stderr,"[master] fork error\n");
		break;
	case 0:
		{
			unsigned int i;
			unsigned int tmpPid;

			friend_msg newNode;
			Message transHop;
			signal(SIGINT,SIG_DFL);

			while (1)
			{
				receive_message(mQueue,&newNode,sizeof(Message),TRANSACTION_MTYPE,0);
				nodesPid[nCounter].Node_state=ALIVE;
				nodePid[nCounter].balance=0;
				tmpPid=generate_node()
			}
			
		}
	default:
		break;
	}
 }

#include "include/Master.h" 
#include "include/Common.h"
#include <stdlib.h>
#define USER_NAME "./Users"
#define USER_NODE "./Nodes"
struct _ConfigParameters *par;
user *state; 
user *userPid;
node *nodesPid;
unsigned int nodeCounter;
unsigned int userCounter;


void Sh_MemMaster( key_t key,size_t size,int shmflg){  
     int m_id; 
     int ShInit=shmget(key,sizeof(SO_REGISTRY_SIZE)*2,IPC_CREAT|0666); /*ShdMem Define Area*/  /* Raddoppio l'area per evitare saturazioni*/
    char*shm=shmat(ShInit,NULL,0); /*Attach ShMem;*/ 
    int shmdet=shmdt(ShInit); /*Detach ShMem*/
    system("./Users");
}


void Sh_UserPID(key_t key,size_t size,int shmflg){ 
 int Sh_UserPIDDet;
 int Sh_UserPIDInit;
 int id; 
 Sh_UserPIDInit=shmget(key,sizeof(ConfigParameters.SO_USER_NUM),IPC_CREAT|0666); 
 char * Sh_UserPIDAttach=shmat(Sh_UserPID,NULL,0); 
 Sh_UserPIDDet=shmdt(Sh_UserPIDInit); 
}


void Shared_Memory( key_t key,size_t size,int shmflg){  
     int m_id; 
     int *shm; 
     int ShInit=shmget(key,sizeof(SO_REGISTRY_SIZE)*2,IPC_CREAT|0666); /*ShdMem Define Area*/  
     shm=shmat(ShInit,NULL,0); /*Attach ShMem;*/ 
     int shmdt(const void *shmaddr); /*Detach ShMem*/
}



/* generate the user with fork and lauch ./users with execve*/
void generateUser() {    /*need to implement uCounter !! */
     int j ; 
     for ( j = 0; j < ConfigParameters.SO_USER_NUM; j++)
     {
          /* code */
     
     
     pid_t uPid=fork();
     switch(uPid){
      case -1 : 
      perror("USER SPAWN ERROR CHECK IT \n"); 
      exit(EXIT_FAILURE); 
      case 0 : 
      userPid[j].usPid=getpid(); 
      userPid[j].balance=0; 
      userPid[j].Us_state=ALIVE; 
      break;
      default: 
      break;  
      } 
     }
}

void generateNode() {
     int i;
     for ( i = 0; i < ConfigParameters.SO_NODES_NUM; i++)
     {
     pid_t nPid=fork();
      switch (nPid)
      {
      case -1:
           perror("NODE'S FAILURE");
           exit(EXIT_FAILURE);
           break;
      case 0:
      nodesPid[i].nodPid=getpid();
      nodesPid[i].balance=0;
      break;
      default:
           break;
      }
     }          
 }


/* Stop Simulation handler Ctrl-C */
void master_Stop_handler(int sigum){
     char c;
     signal(sigum,SIG_IGN);
     printf("you pressed CTRL-C\n" "would you like to quit?[y/n]");
     c=getchar();
     if(c=='y'||c=='Y'){
          exit(0);
     } else {
          signal(SIGINT,master_Stop_handler);
          getchar();
     }
}





int main(){

/* create nodes in base of parameters given */
generateNode();  
generateUser();
signal(SIGINT,master_Stop_handler);
}

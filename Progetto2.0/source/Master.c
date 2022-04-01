#include "include/Master.h" 
#include "include/Common.h"
#include <stdlib.h>
#define USER_NAME "./Users"
#define USER_NODE "./Nodes"
struct ConfigParameters *par;

void Sh_MemMaster( key_t key,size_t size,int shmflg){  
     int m_id; 
     int ShInit=shmget(key,sizeof(So_REGISTRY_SIZE)*2,IPC_CREAT|0666); /*ShdMem Define Area*/  /* Raddoppio l'area per evitare saturazioni*/
    char*shm=shmat(ShInit,NULL,0); /*Attach ShMem;*/ 
    int shmdet=shmdt(ShInit); /*Detach ShMem*/
    system("./Users");
}


void Sh_UserPID(key_t key,size_t size,int shmflg){ 
 
int id; 
int Sh_UserPIDInit=shmget(key,sizeof(par->SO_USER_NUM),IPC_CREAT|0666); 
char * Sh_UserPIDAttach=shmat(Sh_UserPID,NULL,0); 
int Sh_UserPIDDet=shmdt(Sh_UserPIDInit); 
}


void Shared_Memory( key_t key,size_t size,int shmflg){  
     int m_id; 
     int *shm; 
     int ShInit=shmget(key,sizeof(So_REGISTRY_SIZE)*2,IPC_CREAT|0666); /*ShdMem Define Area*/  
     shm=shmat(ShInit,NULL,0); /*Attach ShMem;*/ 
     int shmdt(const void *shmaddr); /*Detach ShMem*/
}
/* generate the user with fork and lauch ./users with execve*/
void generateUser(char *userArgv[],int uCounter) {    /*need to implement uCounter !! */
     pid_t uPid=fork();

     switch(uPid){
          case -1:
               printf("error forking user");
               break;
          case 0:
               execve(USER_NAME,userArgv,NULL);  /*look man execve in case of doubt*/
               break;

          default:
               uPid[uCounter].pid=uPid;
               return;
     }
}

void generateNode(char *nodeArgv[],int nodeCounter) {
     pid_t nPid=fork();
          switch (nPid)
          {
          case -1:
               printf("error in forking user");
               break;
          case 0:
               execve(USER_NODE,nodeArgv,NULL); /*same of generateUser();*/
          default:
               nPid[nodeCounter].pid=nPid;
               break;
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
unsigned int nodeCounter;
unsigned int userCounter;
     signal(SIGINT,master_Stop_handler);
/* create nodes in base of parameters given */
for(nodeCounter=0;nodeCounter<par->SO_NDOES_NUM;nodeCounter++){
     nPid[nodeCounter].status=available;
     nPid[nodeCouter].balance=0;
     generateNode(nodeArgv,nodeCounter);
     
}
/* create user in base of parameters given*/
for(userCounter=0;userCounter<par->SO_USER_NUM;userCounter++){
     uPid[userCounter].status=alive;
     uPid[userCounter.balance]=0;
     generateUser(userArgv,userCounter);
}

}

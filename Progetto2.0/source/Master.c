#include "include/Master.h" 
#
#define USER_NAME "./Users"
#define USER_NODE "./Nodes"


/*
void Shared_Memory( key_t key,size_t size,int shmflg){  
     int m_id; 
     int Init=shmget(IPC_PRIVATE,sizeof(SO_REGISTRY_SIZE)*2,0666); //ShdMem Define Area 
     /*struct Shared_Data = shmat(m_id,NULL,0);   
} */



void  UsersInit(){
     pid_t Parent; 
     pid_t Child; 
     Parent=getppid();
     Child=getpid(); 

}


void NodeInit(){ 
     pid_t NParent;
     pid_t NChild; 
     NParent= getppid(); 
     NChild=getpid();
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
     UsersInit();
     NodeInit();
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
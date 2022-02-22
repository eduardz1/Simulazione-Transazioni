#include "include/Master.h" 





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
int i;
     signal(SIGINT,master_Stop_handler);
     UsersInit();
     NodeInit();
for(i=0;;i++){

}
}
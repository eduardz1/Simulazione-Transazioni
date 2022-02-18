#include <Master.h>






void Shared_Memory( key_t key,size_t size,int shmflg){  
     int m_id; 
     int Init=shmget(IPC_PRIVATE,sizeof(SO_REGISTRY_SIZE)*2,0666); //ShdMem Define Area 
     /*struct Shared_Data = shmat(m_id,NULL,0); */  
}




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
void master_Stop_handle(){
    int status,WPid; 
    signal(SIGINT,/* FUNZIONE DA TERMINARE */); 
     
}





int main(){
     


}
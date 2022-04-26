#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>   /* for signal handler*/
#include <stdlib.h> /*for exit function*/

void master_Stop_handler(int sigum);
/*
struct Transaction  
{
    timestamp; 
    reciver; 
    
}; */

void Sh_MemMaster( key_t key,size_t size,int shmflg); 
void Sh_UserPID(key_t key,size_t size,int shmflg);
void GenerateUser(char userArgv[],int userCounter);
void master_Stop_handle(int signum); 
void Node_Sem_init();

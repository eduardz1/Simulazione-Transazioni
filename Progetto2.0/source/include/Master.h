#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>   /* for signal handler*/
#include <stdlib.h> /*for exit function*/


void Sh_MemMaster( key_t key,size_t size,int shmflg); 
void Sh_UserPID(key_t key,size_t size,int shmflg);
void Shared_Memory( key_t key,size_t size,int shmflg);
void generateUser(int * userArgv[],int uCounter);
void generateNode(int *nodeArgv[],int nodeCounter);
void master_Stop_handle(int signum); 

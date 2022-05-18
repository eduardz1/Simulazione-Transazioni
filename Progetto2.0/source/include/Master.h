#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>   /* for signal handler*/
#include <stdlib.h> /*for exit function*/
pid_t nPid;

void Sh_MemMaster( key_t key,size_t size,int shmflg); 
void Sh_UserPID(key_t key,size_t size,int shmflg);
void Shared_Memory( key_t key,size_t size,int shmflg);
void generateUser();
void generateNode();
void master_Stop_handle(int signum); 

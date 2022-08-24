#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>   /* for signal handler*/
#include <stdlib.h> /*for exit function*/
#include  <string.h>
/*#include "Print.h"*/
#include "../Util/Ms_Queue.h"
pid_t nPid;

void Sh_MemMaster( key_t key,size_t size,int shmflg); 
void Sh_UserPID(key_t key,size_t size,int shmflg);
void Shared_Memory( key_t key,size_t size,int shmflg);
void generateUser();
void generateNode();
void signal_handler(int signum);



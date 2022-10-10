#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h> /*for exit function*/
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "Common.h"
#include "../Util/Ms_Queue.h"
pid_t nPid;

void Sh_MemMaster( key_t key,size_t size,int shmflg); 
void Sh_UserPID(key_t key,size_t size,int shmflg);
void Shared_Memory( key_t key,size_t size,int shmflg);
void create_arguments(int* IPC_array, char** argv);
void make_ipc_array(int *IPC_array);
void generateUser(int uCounter , char * userArgv[]);
void generateNode(int nCounter , char * nodeArgv[]);
void shared_memory_objects_init(int *shmArray);
void sems_init();
int message_queue_id();
/*singal handler to end the simulation */
void signal_handler(int signum);


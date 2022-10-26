/* #include "../Util/Ms_Queue.h"*/ 


#ifdef USERS_H_INCLUDE
#define USERS_H_INCLUDE
#include <sys/types.h>
#include <time.h>
#include "../include/Conf.h"
#include "../Util/IPCS_SEM.h"
#include "../Util/lklist.h"


void Sh_MemUser(key_t key ,size_t size,int shmflg);

int getPidUserIndex(int searchPid);
void updateStatus(int setStatus); 
void start_transaction(pid_t userPid,int money, int reward);
void current_balance();

int send_message(int queueID,void *msg,int size,int flag);
#endif
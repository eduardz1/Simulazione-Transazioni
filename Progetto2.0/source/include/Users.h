#include "../Util/Ms_Queue.h"   
#include "../include/Common.h"
#include <sys/types.h>
#include <time.h>
#include "../include/Conf.h"
#include "../Util/IPCS_SEM.h"
/*
struct node {
  transaction trans;
  struct node *next;
};
 */



void Sh_MemUser(key_t key ,size_t size,int shmflg);

int getPidUserIndex(int searchPid);
void updateStatus(int setStatus); 
void start_transaction(int money, int reward);
void current_balance();

int send_message(int queueID,void *msg,int size,int flag);

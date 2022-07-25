#include "../Util/Ms_Queue.h"
#include <sys/types.h>
#include <time.h>
#include "../include/Common.h"

struct node {
  transaction trans;
  struct node *next;
};



void Sh_MemUser(key_t key ,size_t size,int shmflg);

int getPidUserIndez(int searchPid);
void updateStatus(int setStatus); 
void start_transaction(pid_t userPid , int money, int reward);
void Kill_User(int signo); 
void Alive_User(pid_t myPid , int money , user * UserStatus);
void CurrentBalance(user * UserBalance , pid_t myPid , transaction * PidSender);


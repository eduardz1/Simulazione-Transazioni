#ifndef COMMON_H_INCLUDE
#define COOMON_H_INCLUDE
#endif
#ifndef  _GNU_SOURCE
#define  _GNU_SOURCE
#endif

#ifndef NULL
#define NULL 0
#endif





#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <pthread.h> 
#include <time.h>
#include "Conf.h"

#ifndef MS_QUEUE
#endif 
#define SO_REGISTRY_SIZE 100

#define TRANSACTION_MTYPE 28410
#define SO_BLOCK_SIZE 10
#define NO_BLOCK -1 

#define IPC_ERROR -1
#define SHM_PARAMETERS 1337
#define SHM_USERS_ARRAY 1338
#define SHM_NODES_ARRAY 1339
#define SHM_LEDGER 1340
#define SEM_USERS_PIDS_KEY 420
#define SEM_NODES_PIDS_KEY 421
#define SEM_LEDGER_KEY 422
#define M_QUEUE_KEY 0x5AD

/* -- ARGV LOCATION OF IPC OBJECTS -- */
#define USERS_PID_ARGV (atoi(argv[1]))
#define NODES_PID_ARGV (atoi(argv[2]))
#define PARAMETERS_ARGV (atoi(argv[3]))
#define LEDGER_ARGV (atoi(argv[4]))
#define SEM_USERS_PIDS_ARGV (atoi(argv[5]))
#define SEM_LEDGER_ARGV (atoi(argv[6]))
#define SEM_NODES_PIDS_ARGV (atoi(argv[7]))


/*USERS STATE MACRO's */

#define PROBLEM -1 
#define OK 0
#define NOT_ARRIVED 1 
#define MAX_RETRY 2 


/*
typedef struct ConfigParameters
{
    unsigned int SO_USER_NUM  ;
    unsigned int SO_NODES_NUM ;
    unsigned int SO_BUDGET_INIT;
    char SO_REWARD; 
    unsigned long SO_MIN_TRANS_GEN_NSEC;
    unsigned long SO_MAX_TRANS_GEN_NSEC;
    unsigned int SO_RETRY;
    unsigned int SO_TP_SIZE;
    unsigned long SO_MIN_TRANS_PROC_NSEC;
    unsigned long SO_MAX_TRANS_PROC_NSEC;
    unsigned int SO_SIM_SEC;
    unsigned int SO_FRIENDS_NUM;
    unsigned int SO_HOPS;
}configparameter;
extern configparameter ConfigParameters; 

*/
typedef struct UserStatus
{
    pid_t usPid; 
    unsigned int balance; 
    enum  Us_status  
    { 
      ALIVE,
      NOTAVAILBLE,
      DEAD
    }Us_state; 
}user;


typedef  struct msg_friend{
    long mtype;
    pid_t friend ;
}friend_msg ; 
 




typedef struct MoneyTransaction
{
    struct timespec time;
    pid_t Sender; 
    pid_t Receiver; 
    int Money; 
    int Reward; 
    enum T_status
    { 
       pending,
       processing,
       confirmed,
       aborted 
 }MoneyStatusTrans; 
 
}transaction;

/* moved here from Ms_queue.h to avoid conflict errors*/
typedef struct msgbuf_trans
{ 
  size_t Size_Msg; 
  int  m_type;
  char mesText[1];
    struct message
    {
      int hops; 
      transaction uTrans;
      struct msgbuf_trans *next;
    }Message_Transaction;
}Message;

/*simulation node struct*/
typedef struct StructNode
{

    pid_t nodPid;
    unsigned long balance;
    int tpsize;
    enum Node_stat {
        available,
        full
    }Node_state;

}node_t;

/* linked list nodes struct*/
struct node
{
		transaction trans;
		struct node *next;
};

typedef struct Block
  {
    transaction t_list[SO_BLOCK_SIZE];
    unsigned int blockIndex; /*the index need to be updated when a block is written */
  }Block_;


#ifndef COMMON_H_INCLUDE
#define COOMON_H_INCLUDE
#endif
#ifndef  _GNU_SOURCE
#define  _GNU_SOURCE
/*#include <bits/types/struct_timespec.h>*/
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


#ifndef MS_QUEUE
#endif 
#define SO_REGISTRY_SIZE 100

#define TRANSACTION_MTYPE 2800 
#define SO_BLOCK_SIZE 10

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
      NOTAVAIBLE,
      DEAD
    }Us_state; 
}user;

 





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

typedef struct StructNode
{

    pid_t nodPid;
    unsigned long balance;
    struct Node *head;
    struct Node *next;
    transaction * transaction;
    enum Node_stat {
        available,
        full
    }Node_state;

}node;


typedef struct Block
  {
    transaction t_list[SO_BLOCK_SIZE];
    unsigned int blockIndex; /*the index need to be updated when a block is written */
    char tmpLedger[]; 
  }Block_;


#ifndef  _GNU_SOURCE
#define  _GNU_SOURCE
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
#include <signal.h>
#include <limits.h>



struct ConfigParameters
{
    unsigned int SO_USER_NUM;
    unsigned int SO_NODES_NUM;
    unsigned int SO_BUDGET_INIT;
    char SO_REWARD; /* max value 100 */
    unsigned long SO_MIN_TRANS_GEN_NSEC;
    unsigned long SO_MAX_TRANS_GEN_NSEC;
    unsigned int SO_RETRY;
    unsigned int SO_TP_SIZE;
    unsigned long SO_MIN_TRANS_PROC_NSEC;
    unsigned long SO_MAX_TRANS_PROC_NSEC;
    unsigned int SO_SIM_SEC;
    unsigned int SO_FRIENDS_NUM;
    unsigned int SO_HOPS;
};


typedef struct UserStatus
{
    pid_t UsPid; 
    unsigned int balance; 
    enum                       /* Enum: ASSEGNA NOMI A COSTANTI--> Ogni costante ha una flag numerata "NON VISIBILE " */
    { 
      alive,
      NotAvailble,
      dead 
    }status; 
}user;



typedef struct MoneyTransaction
{
    pid_t Sender; 
    pid_t Reciver; 
    int Money; 
    int Reward; 
    enum
    { 
       pending,
       processing,
       confirmed,
       aborted, 

    } status; 
}transaction;


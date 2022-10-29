#ifndef STRUCT_H_TEST
#define STRUCT_H_TEST
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
#define MSGSIZE 3213
struct parametri para;
typedef struct message 
{
   
    long mytpe; 
    //char mtext[100]; 
    pid_t pid ; 
    int reward; 
    int bool; 
}msg;


struct parametri
{
    pid_t myPid; 
    int reward; 
    int bool;
};


#endif
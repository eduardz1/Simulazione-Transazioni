#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#define SO_REGISTRY_SIZE 
#define USER_NAME
#define NODE_NAME
#define SO_USERS_NUM 
#define SO_NODES_NUM 


void Shared_Memory( key_t key,size_t size,int shmflg); 
void UsersInit(); 
void UserInit(); 
void NodeInit(); 
void master_Stop_handle(); 

struct Shared_Data
{
    unsigned long indx_write; 
    
    pid_t Pid_UserSHM[SO_USERS_NUM];
     
};





struct Transaction  
{
    timestamp; 
    reciver; 
    MoneySend; 
    Reward; 
};








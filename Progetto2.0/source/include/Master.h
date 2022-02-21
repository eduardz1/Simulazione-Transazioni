#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>   /* for signal handler*/
#include <stdlib.h> /*for exit function*/

void master_Stop_handler(int sigum);
/*
struct Transaction  
{
    timestamp; 
    reciver; 
    
}; */
#include <time.h>
#include "nodes.h"
#include "common.h"


#define SO_TP_SIZE (argv[10]) 
#define SO_MIN_TRANS_PROC_NSEC (argv[11])
#define SO_MAX_TRANS_PROC_NSEC (argv[12])
#define SO_TP_SIZE
#define SO_BLOCK_SIZE
#define SO_MIN_TRANS_PROC_NSEC (atol(argv[8]))
#define SO_MAX_TRANS_PROC_NSEC (atol(argv[9]))
/* transaction pool==transaction's array */

int main(int argc, char *argv[]){
    int myPID = getpid();
    printf("Node %d has finished\n", myPID);
    return;
}



void Node(){
    int t_pool[SO_TP_SIZE];
    checkTpFull(t_pool[SO_TP_SIZE]);
    arrayProcesser();
    createBlock();
}

int checkTpFull(int t_pool[SO_TP_SIZE]){
    if(t_pool[SO_TP_SIZE]==SO_TP_SIZE) {
        return 0;
    }
}

void arrayProcesser(){
int i=0;
for(i;i<SO_TP_SIZE-1;i++){


    }
}

int createBlock(){
    
}
    



int sleepMethod(int argc, char *argv[]){
 randSleepTime.tv_sec = 0;
    randSleepTime.tv_nsec = RAND(SO_MIN_TRANS_PROC_NSEC, SO_MAX_TRANS_PROC_NSEC);
}
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
void Node(){
    int i=0;
    int t_pool[SO_TP_SIZE];

    for(i;i<SO_TP_SIZE-1;i++){
        

    }
}


int main(int argc, char *argv[]){
 randSleepTime.tv_sec = 0;
    randSleepTime.tv_nsec = RAND(SO_MIN_TRANS_PROC_NSEC, SO_MAX_TRANS_PROC_NSEC);
}
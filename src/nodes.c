#include <time.h>
#include "include/nodes.h"
#include "include/common.h"

/* transaction pool==transaction's array */
/*
void Node(argv[2])
{
    int t_pool[SO_TP_SIZE];
    check_Tp_Full(t_pool[SO_TP_SIZE]);
    array_Processer(argv[2]);
    create_Block();
}

int check_Tp_Full(int t_pool[SO_TP_SIZE])
{
    if (t_pool[SO_TP_SIZE] == SO_TP_SIZE)
    {
        return 0;
    }
}

void array_Processer()
{
    int i = 0;
    for (i; i < SO_TP_SIZE - 1; i++)
    {
        
    }
}

int createBlock()
{

    
}

int sleepMethod(int argc, char *argv[])
{
    randSleepTime.tv_sec = 0;
    randSleepTime.tv_nsec = RAND(SO_MIN_TRANS_PROC_NSEC, SO_MAX_TRANS_PROC_NSEC);
}
*/
int main(int argc, char *argv[])
{
    int myPID = getpid();
    printf("Node %d has finished\n", myPID);
    return 0;
}
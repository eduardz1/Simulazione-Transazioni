#include <time.h>
#include "include/nodes.h"
#include "include/common.h"

/* transaction pool==transaction's array */

/*
 ======================
 || GLOBAL VARIABLES ||
 ======================
 */

/* parameters of simulation */
struct parameters *par;
user *usersPID;
node *nodesPID;
ledger *mainLedger;

int semID;
int queueID;

pid_t myPID;

/*void Node()
{
    int t_pool[SO_TP_SIZE];
    checkTpFull(t_pool[SO_TP_SIZE]);
    arrayProcesser();
    createBlock();
}

int checkTpFull(int t_pool[SO_TP_SIZE])
{
    if (t_pool[SO_TP_SIZE] == SO_TP_SIZE)
    {
        return 0;
    }
}

void arrayProcesser()
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
}*/

int main(int argc, char *argv[])
{
    int myPID = getpid();
    printf("Node %d has finished\n", myPID);
    return 0;

    queueID = msgget(myPID, IPC_CREAT | 0600);
}
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

/* initializes signal handlers for SIGINT *
void signal_handler_init(struct sigaction *saINT)
{
    saINT->sa_handler = node_interrupt_handle;
    sigaction(SIGINT, saINT, NULL);
} */

/* CTRL-C handler */
void node_interrupt_handle(int signum)
{
    write(1, "::Node:: SIGINT received\n", 26);
    msgctl(queueID, IPC_RMID, NULL);
    TRACE((":node: queue removed\n"))
    TEST_ERROR
    exit(0);
}

int main(int argc, char *argv[])
{
    int myPID = getpid();
    struct sigaction saINT_node;
    bzero(&saINT_node, sizeof(saINT_node));
    saINT_node.sa_handler = node_interrupt_handle;
    sigaction(SIGINT, &saINT_node, NULL);
    /*
    signal_handler_init(&saINT);*/

    queueID = msgget(myPID, IPC_CREAT | 0600);
    while(1){
        sleep(1);
    }
}
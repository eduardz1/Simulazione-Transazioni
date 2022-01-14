#include "include/common.h"
#include "include/nodes.h"

/* transaction pool==transaction's array */

/*
 ======================
 || GLOBAL VARIABLES ||
 ======================
 */

transaction *pool;

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

/* attaches ipc objects based on IDs passed via arguments */
void attach_ipc_objects(char **argv)
{
    par = shmat(PARAMETERS_ARGV, NULL, 0);
    TRACE((":user %d par->SO_RETRY %d\n", myPID, par->SO_RETRY))
    TEST_ERROR
    usersPID = shmat(USERS_PID_ARGV, NULL, 0);
    TRACE((":user: %d usersPID[0] = %d, usersPID[3] = %d\n", myPID, usersPID[0], usersPID[3]))
    TEST_ERROR
    nodesPID = shmat(NODES_PID_ARGV, NULL, 0);
    TRACE((":user: %d nodesPID[0] = %d, nodesPID[3] = %d\n", myPID, nodesPID[0], nodesPID[3]))
    TEST_ERROR
    mainLedger = shmat(LEDGER_ARGV, NULL, 0);
    TEST_ERROR
    semID = SEM_ID_ARGV;
    TRACE((":user: %d semID is %d\n", myPID, semID));
}

/* initializes signal handlers for SIGINT */
void signal_handler_init(struct sigaction *saINT_node)
{
    saINT_node->sa_handler = node_interrupt_handle;
    sigaction(SIGINT, saINT_node, NULL);
}

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

    struct timespec randSleepTime;
    struct timespec sleepTimeRemaining;
    struct sigaction saINT_node;

    myPID = getpid();

    bzero(&saINT_node, sizeof(saINT_node));

    attach_ipc_objects(argv);
    signal_handler_init(&saINT_node); /* no idea why it isn't working, it's literally the same implementation as user */
    TRACE((":node: %d sighandler init\n", myPID));

    queueID = msgget(myPID, IPC_CREAT | 0600);
    TEST_ERROR
    TRACE((":node: %d queueID is %d\n", myPID, queueID))
    while (1)
    {
        SLEEP_TIME_SET
        /* check if we have at least SO_BLOCK_SIZE-1 transactions in pool
         * if true take them, set status to processing and create new block
         * newBlock = new_block(transactionsChosen);
         * LOCK ledger
         * -- append transaction to ledger setting newBlock.prev equals to last
         * -- block with NULL next in ledger and setting the former block's next
         * -- to *newBlock
         * set every newBlock transaction's status to confirmed
         */
        SLEEP
        /* UNLOCK ledger */
    }
}
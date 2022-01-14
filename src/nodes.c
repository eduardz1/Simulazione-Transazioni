#include <time.h>
#include "include/nodes.h"
#include "include/common.h"

/* transaction pool==transaction's array */

/*
 ======================
 || GLOBAL VARIABLES ||
 ======================
 */
 transaction *t_pool;

/* parameters of simulation */
struct parameters *par;
user *usersPID;
node *nodesPID;
ledger *mainLedger;

int semID;
int queueID;

pid_t myPID;




/*
int check_Tp_Full()
{

    if (t_pool == par->SO_TP_SIZE)
    {
        return 0;
    } else {
        createBlock();            /* if the t_pool isn't full it create new block *      
    }
}
*/



void sleepMethod(int argc, char *argv[])
{
    randSleepTime.tv_sec = 0;
    randSleepTime.tv_nsec = RAND(par->SO_MIN_TRANS_PROC_NSEC, par->SO_MAX_TRANS_PROC_NSEC);
}
void node_interrupt_handle(int signum)
{
    write(1, "::Node:: SIGINT received\n", 26);
    msgctl(queueID, IPC_RMID, NULL);
    TRACE((":node: queue removed\n"))
    TEST_ERROR
    exit(0);
}

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

void signal_handler_init(struct sigaction *saINT_node)
{
    saINT_node->sa_handler = node_interrupt_handle;
    sigaction(SIGINT, saINT_node, NULL);
}

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
    transaction** sumBlock;
    transaction** blockTransaction;
   
    int myPID = getpid();
    struct sigaction saINT_node;
     t_pool=malloc(sizeof(transaction)*(par->SO_TP_SIZE));
    bzero(&saINT_node, sizeof(saINT_node));
    saINT_node.sa_handler = node_interrupt_handle;
    sigaction(SIGINT, &saINT_node, NULL);
    /*
    signal_handler_init(&saINT);*/
   
    queueID = msgget(myPID, IPC_CREAT | 0600);


    while(1){
   
    
    /*check_Tp_Full();*/
    sum_reward(sumBlock);
    new_block(blockTransaction);
    }
}
#include "include/common.h"
#include "include/master.h"

/* -- CL PARAMETERS INITIALIZATION -- */
#define SO_USER_NUM (atoi(argv[1]))
#define SO_NODES_NUM (atoi(argv[2]))
#define SO_NUM_FRIENDS (atoi(argv[3]))
#define SO_SIM_SEC (atoi(argv[4]))

/* -- USER CL PARAMETERS -- */
#define SO_BUDGET_INIT (argv[5])
#define SO_REWARD (argv[6])
#define SO_RETRY (argv[7])
#define SO_MIN_TRANS_GEN_NSEC (argv[8])
#define SO_MAX_TRANS_GEN_NSEC (argv[9])

/* -- NODE CL PARAMETERS -- */
#define SO_TP_SIZE (argv[10])
#define SO_BLOCK_SIZE (argv[11])
#define SO_MIN_TRANS_PROC_NSEC (argv[12])
#define SO_MAX_TRANS_PROC_NSEC (argv[13])

char **user_CL_parameters(char *argv[])
{
    char *userArgv[6];

    userArgv[0] = SO_BUDGET_INIT;
    userArgv[1] = SO_REWARD;
    userArgv[2] = SO_RETRY;
    userArgv[3] = SO_MIN_TRANS_GEN_NSEC;
    userArgv[4] = SO_MAX_TRANS_GEN_NSEC;

    return userArgv;
}

char **node_CL_parameters(char *argv[])
{
    char *nodeArgv[4];

    nodeArgv[0] = SO_TP_SIZE;
    nodeArgv[1] = SO_BLOCK_SIZE;
    nodeArgv[2] = SO_MIN_TRANS_PROC_NSEC;
    nodeArgv[3] = SO_MAX_TRANS_PROC_NSEC;

    return nodeArgv;
}

pid_t spawn_user(char *userArgv[], int shmID)
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        /* error handling goes here */
        break;

    case 0: /* Child case */
        execve("user", userArgv, NULL);
        break;

    default:
        return myPID;
    }
}

pid_t spawn_node(char *nodeArgv[], int shmID)
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        /* error handling goes here */
        break;

    case 0: /* Child case */
        execve("node", nodeArgv, NULL);
        break;

    default:
        return myPID;
    }
}

void interrupt_handle(signum)
{
    /* shmctl(shmID, IPC_RMID, NULL); */
}

int main(int argc, char *argv[])
{
    int shmID = 0; /* dummy value */
    int uCounter, nCounter;
    unsigned int simTime;

    pid_t myPID = getpid();
    pid_t myPPID;

    struct sigaction sa; /* we need to define an handler for CTRL-C command that closes any IPC object */

    /* -- PID ARRAYS -- */
    pid_t *arrayUserPID = malloc(SO_USER_NUM * sizeof(pid_t));
    pid_t *arrayNodesPID = malloc(SO_NODES_NUM * sizeof(pid_t));

    simTime = SO_SIM_SEC;

    ledger_init();

    /* -- SIGNAL HANDLER --
     * first set all bytes of sigation to 0
     * then initialize sa.handler to a pointer to the function interrupt_handle
     * then set the handler to handle SIGINT signals ((struct sigaction *oldact) = NULL)
     */
    bzero(&sa, sizeof(sa));
    sa.sa_handler = interrupt_handle;
    sigaction(SIGINT, &sa, NULL);

    for (uCounter; uCounter < SO_USER_NUM; uCounter++)
    {
        arrayUserPID[uCounter] = spawn_user(user_CL_parameters(argv), shmID);
    }

    for (nCounter; nCounter < SO_NODES_NUM; nCounter++)
    {
        arrayNodesPID[nCounter] = spawn_node(node_CL_parameters(argv), shmID);
    }

    wait(simTime);
    kill(mypid(), SIGINT); /* our sigint handler needs to do quite a lot of things to print the wall of test below */

    print_user_nodes_table(myPID, arrayUserPID, arrayNodesPID, SO_USER_NUM, SO_NODES_NUM);
    print_kill_signal();                /* need to define, prints reason of termination (simTime elapsed/
                                                                                         ledger full/
                                                                                         all processes terminated) */
    print_user_balance();               /* need to define, prints balance of every user */
    print_node_balance();               /* need to define, prints balance of every node */
    print_num_aborted();                /* need to define, prints num of processes aborted */
    print_num_blocks();                 /* need to define, prints num of blocks saved in the ledger */
    print_transactions_still_in_pool(); /* need to define, prints num of transactions still in the pool of each node */

    return 0;
}
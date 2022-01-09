#include "include/common.h"
#include "include/master.h"
#include "include/print.h"
#include "include/parser.h"

/* -- CL PARAMETERS INITIALIZATION -- *
#define SO_USER_NUM (atoi(argv[1]))
#define SO_NODES_NUM (atoi(argv[2]))
#define SO_NUM_FRIENDS (atoi(argv[3]))
#define SO_SIM_SEC (atoi(argv[4]))

/* -- USER CL PARAMETERS -- *
#define SO_BUDGET_INIT (argv[5])
#define SO_REWARD (argv[6])
#define SO_RETRY (argv[7])
#define SO_MIN_TRANS_GEN_NSEC (argv[8])
#define SO_MAX_TRANS_GEN_NSEC (argv[9])

/* -- NODE CL PARAMETERS -- *
#define SO_TP_SIZE (argv[10])
#define SO_MIN_TRANS_PROC_NSEC (argv[11])
#define SO_MAX_TRANS_PROC_NSEC (argv[12]) */

#define USER_NAME "./user"
#define NODE_NAME "./node"

char **arguments(int *IPC_array)
{
    char *argv[10];

    char *uPID_array = malloc(3 * sizeof(IPC_array[0]) + 1);
    char *nPID_array = malloc(3 * sizeof(IPC_array[0]) + 1);
    char *parameters = malloc(3 * sizeof(IPC_array[0]) + 1);

    sprintf(uPID_array, "%d", IPC_array[0]);
    sprintf(nPID_array, "%d", IPC_array[1]);
    sprintf(parameters, "%d", IPC_array[2]);

    argv[0] = USER_NAME;
    argv[1] = uPID_array;
    argv[2] = nPID_array;
    argv[3] = parameters;

    return argv;
}

pid_t spawn_user(char *userArgv[])
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        /* error handling goes here */
        break;

    case 0: /* Child case */
        execve(USER_NAME, userArgv, NULL);
        break;

    default:
        return myPID;
    }
}

pid_t spawn_node(char *nodeArgv[])
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        /* error handling goes here */
        break;

    case 0: /* Child case */
        execve(NODE_NAME, nodeArgv, NULL);
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
    int uCounter = 0, nCounter = 0;
    int IPC_array[3] = {0};
    unsigned int simTime;

    struct sigaction sa; /* we need to define an handler for CTRL-C command that closes any IPC object */
    struct parameters par = parser();

    pid_t myPID = getpid();

    pid_t *usersPID = (pid_t *)malloc(par.SO_USER_NUM * sizeof(pid_t));
    pid_t *nodesPID = (pid_t *)malloc(par.SO_NODES_NUM * sizeof(pid_t));

    int usersPID_ID = shmget(IPC_PRIVATE, ARRAY_SIZE(usersPID), 0600);
    int nodesPID_ID = shmget(IPC_PRIVATE, ARRAY_SIZE(nodesPID), 0600);
    int par_ID = shmget(IPC_PRIVATE, sizeof(par), 0600);

    usersPID = shmat(usersPID_ID, NULL, 0);
    nodesPID = shmat(nodesPID_ID, NULL, 0);
    par = shmat(par_ID, NULL, 0);
    shmctl(usersPID_ID, IPC_RMID, NULL);
    shmctl(nodesPID_ID, IPC_RMID, NULL);
    shmctl(par_ID, IPC_RMID, NULL);

    IPC_array[0] = usersPID_ID;
    IPC_array[1] = nodesPID_ID;
    IPC_array[2] = par_ID;

    simTime = par.SO_SIM_SEC;

    /*ledger_init(); SEG FAULT RN */

    /* -- SIGNAL HANDLER --
     * first set all bytes of sigation to 0
     * then initialize sa.handler to a pointer to the function interrupt_handle
     * then set the handler to handle SIGINT signals ((struct sigaction *oldact) = NULL)
     */
    bzero(&sa, sizeof(sa));
    sa.sa_handler = interrupt_handle;
    sigaction(SIGINT, &sa, NULL);

    for (uCounter; uCounter < par.SO_USER_NUM; uCounter++)
    {
        usersPID[uCounter] = spawn_user(arguments(IPC_array));
    }

    for (nCounter; nCounter < par.SO_NODES_NUM; nCounter++)
    {
        nodesPID[nCounter] = spawn_node(arguments(IPC_array));
    }

    if (getpid() != myPID)
        return;

    wait(simTime);
    /* kill(myPID, SIGINT); /* our sigint handler needs to do quite a lot of things to print the wall of test below */

    print_user_nodes_table(myPID, usersPID, nodesPID, par.SO_USER_NUM, par.SO_NODES_NUM);
    /* print_kill_signal();                /* need to define, prints reason of termination (simTime elapsed/
                                                                                         ledger full/
                                                                                         all processes terminated) *
    print_user_balance();               /* need to define, prints balance of every user *
    print_node_balance();               /* need to define, prints balance of every node *
    print_num_aborted();                /* need to define, prints num of processes aborted *
    print_num_blocks();                 /* need to define, prints num of blocks saved in the ledger *
    print_transactions_still_in_pool(); /* need to define, prints num of transactions still in the pool of each node */

    return 0;
}
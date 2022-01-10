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

#define USER_NAME "./users"
#define NODE_NAME "./nodes"

void makeArguments(char **argv, int *IPC_array)
{
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
}

pid_t spawn_user(char *userArgv[])
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        printf("Error creating fork for user\n");
        break;

    case 0: /* Child case */
        printf("Spawning child\n");
        execve(USER_NAME, userArgv, NULL);
        TEST_ERROR;
        printf("Non puoi vedere mai questo messagio, spero\n");
        break;

    default:
        printf("User myPID: %d\n", myPID);
        return myPID;
    }
}

pid_t spawn_node(char *nodeArgv[])
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        printf("Error creating fork for node\n");
        break;

    case 0: /* Child case */
        printf("Spawning node\n");
        execve(NODE_NAME, nodeArgv, NULL);
        TEST_ERROR;
        printf("Non puoi vedere mai questo messagio, spero\n");
        break;

    default:
        printf("Node myPID: %d\n", myPID);
        return myPID;
    }
}

void interrupt_handle(signum)
{
    /* shmctl(shmID, IPC_RMID, NULL); */
}

int main(int argc, char *argv[])
{
    pid_t myPID = getpid();

    int uCounter, nCounter;
    int IPC_array[3] = {0};
    unsigned int simTime;
    char *argvSpawns[10];

    pid_t *usersPID; /* Array of users' PIDs */
    pid_t *nodesPID; /* Array of nodes' PIDs */

    struct sigaction sa; /* we need to define an handler for CTRL-C command that closes any IPC object */
    struct sembuf sops;
    struct parameters *par;

    /* shared memory segments IDs */
    int usersPID_ID;
    int nodesPID_ID;
    int par_ID;

    par_ID = shmget(IPC_PRIVATE, sizeof(par), 0600);
    par = (struct parameters *)shmat(par_ID, NULL, 0);
    /*just there until parser works*/
    par->SO_USER_NUM = 10;
    par->SO_NODES_NUM = 5;
    par->SO_BUDGET_INIT = 1000;
    par->SO_REWARD = 1;
    par->SO_MIN_TRANS_GEN_NSEC = 100000000;
    par->SO_MAX_TRANS_GEN_NSEC = 200000000;
    par->SO_RETRY = 20;
    par->SO_TP_SIZE = 1000;
    par->SO_MIN_TRANS_PROC_NSEC = 100000000;
    par->SO_MAX_TRANS_PROC_NSEC = 200000000;
    par->SO_SIM_SEC = 10;
    par->SO_FRIENDS_NUM = 3;
    par->SO_HOPS = 10;

    calloc(usersPID, ((par->SO_USER_NUM) * sizeof(pid_t)));
    calloc(nodesPID, ((par->SO_NODES_NUM) * sizeof(pid_t)));
    usersPID_ID = shmget(IPC_PRIVATE, sizeof(usersPID), 0600);
    nodesPID_ID = shmget(IPC_PRIVATE, sizeof(nodesPID), 0600);
    usersPID = (pid_t *)shmat(usersPID_ID, NULL, 0);
    nodesPID = (pid_t *)shmat(nodesPID_ID, NULL, 0);

    shmctl(usersPID_ID, IPC_RMID, NULL);
    shmctl(nodesPID_ID, IPC_RMID, NULL);
    shmctl(par_ID, IPC_RMID, NULL);

    printf("SO_USER_NUM: %d\n", par->SO_USER_NUM);
    printf("Initialized every shared memeory segment\n");

    IPC_array[0] = usersPID_ID;
    IPC_array[1] = nodesPID_ID;
    IPC_array[2] = par_ID;



    simTime = par->SO_SIM_SEC;

    /*ledger_init(); SEG FAULT RN */

    /* -- SIGNAL HANDLER --
     * first set all bytes of sigation to 0
     * then initialize sa.handler to a pointer to the function interrupt_handle
     * then set the handler to handle SIGINT signals ((struct sigaction *oldact) = NULL)
     */
    bzero(&sa, sizeof(sa));
    sa.sa_handler = interrupt_handle;
    sigaction(SIGINT, &sa, NULL);

    printf("SO_USER_NUM: %d\n", par->SO_USER_NUM);
    printf("Sig handler done\n");

    makeArguments(argvSpawns, IPC_array);

    printf("Made arguments\n");

    printf("SO_USER_NUM: %d\n", par->SO_USER_NUM);
    for (uCounter = 0; uCounter < par->SO_USER_NUM; uCounter++)
    {
        usersPID[uCounter] = spawn_user(argvSpawns);
        if (getpid() != myPID)
            return;
    }
    printf("SO_NODES_NUM: %d\n", par->SO_NODES_NUM);
    for (nCounter = 0; nCounter < par->SO_NODES_NUM; nCounter++)
    {
        nodesPID[nCounter] = spawn_node(argvSpawns);
        if (getpid() != myPID)
            return;
    }

    if (getpid() != myPID)
        return;

    sleep(simTime);

    printf("The time has come\n");
    /* kill(myPID, SIGINT); /* our sigint handler needs to do quite a lot of things to print the wall of test below */
    printf("User %d\n", usersPID[0]);
    printf("User %d\n", usersPID[1]);
    printf("User %d\n", usersPID[2]);
    printf("User %d\n", usersPID[3]);
    printf("User %d\n", usersPID[4]);
    printf("User %d\n", usersPID[5]);
    printf("User %d\n", usersPID[6]);
    printf("User %d\n", usersPID[7]);
    printf("User %d\n", usersPID[8]);
    printf("User %d\n", usersPID[9]);
    printf("Node %d\n", nodesPID[0]);
    printf("Node %d\n", nodesPID[1]);
    printf("Node %d\n", nodesPID[2]);
    printf("Node %d\n", nodesPID[3]);
    printf("Node %d\n\n", nodesPID[4]);

    print_user_nodes_table(myPID, usersPID, nodesPID, par->SO_USER_NUM, par->SO_NODES_NUM);
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
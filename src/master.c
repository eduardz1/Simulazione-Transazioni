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
    argv[10] = NULL; /* Terminating argv with NULL value */
}

pid_t spawn_user(char *userArgv[])
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        printf("-- Error forking for user\n");
        break;

    case 0: /* Child case */
#ifdef VERBOSE
        printf("-- Spawning child\n");
#endif
        execve(USER_NAME, userArgv, NULL);
        TEST_ERROR
        printf("!! Non puoi vedere mai questo messagio, spero\n");
        break;

    default:
#ifdef VERBOSE
        printf("-- User myPID: %d\n", myPID);
#endif
        return myPID;
    }
}

pid_t spawn_node(char *nodeArgv[])
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        printf("!! Error forking for node\n");
        break;

    case 0: /* Child case */
#ifdef VERBOSE
        printf("-- Spawning node\n");
#endif
        execve(NODE_NAME, nodeArgv, NULL);
        TEST_ERROR
        printf("!! Non puoi vedere mai questo messagio, spero\n");
        break;

    default:
#ifdef VERBOSE
        printf("-- Node myPID: %d\n", myPID);
#endif
        return myPID;
    }
}

void master_interrupt_handle(int signum)
{/*
    int status;
    int res_kill;
    int i = 0;

    pritnf("-- CTRL-C killing program");

    for (i = 0; i < par->SO_NODES_NUM; i++)
    {
        /*if (childs[i].status == 1)
        {
            res_kill = kill(childs[1].pid, SIGINT); /* kill all childs*
        }*
    }
    while (wait(&status) != -1)
    {
        status >> 8;
    }
    /*semctl(semid, 0, IPC_RMID);     /*deleting mempid_sem *
    /*shmtcl(pidmem_id, IPC_RMID, 0); /* deleting shared memory segment*
    exit(1);*/
    exit(0);
} 

int main(int argc, char *argv[])
{
    pid_t myPID = getpid();

    int uCounter, nCounter, returnVal;
    int IPC_array[3] = {0};
    unsigned int simTime;
    char *argvSpawns[10] = {0};

    user *usersPID;
    node *nodesPID;

    struct sigaction sa; /* we need to define an handler for CTRL-C command that closes any IPC object */
    struct sembuf sops;
    struct parameters *par;

    /* shared memory segments IDs */
    int usersPID_ID;
    int nodesPID_ID;
    int par_ID;

    par_ID = shmget(SHM_PARAMETERS, sizeof(par), IPC_CREAT | 0600);
    par = (struct parameters *)shmat(par_ID, NULL, 0);
    if (parseParameters(par) == CONF_ERROR)
        printf("-- Error reading conf file, defaulting to conf#1\n");

    calloc(usersPID, ((par->SO_USER_NUM) * sizeof(user)));
    calloc(nodesPID, ((par->SO_NODES_NUM) * sizeof(node)));
    usersPID_ID = shmget(SHM_USERS_ARRAY, sizeof(usersPID), IPC_CREAT | 0600);
    nodesPID_ID = shmget(SHM_NODES_ARRAY, sizeof(nodesPID), IPC_CREAT | 0600);
    usersPID = (user *)shmat(usersPID_ID, NULL, 0);
    nodesPID = (node *)shmat(nodesPID_ID, NULL, 0);

    shmctl(usersPID_ID, IPC_RMID, NULL);
    shmctl(nodesPID_ID, IPC_RMID, NULL);
    shmctl(par_ID, IPC_RMID, NULL);

#ifdef VERBOSE
    printf("-- Initialized every shared memory segment\n");
#endif

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
    sa.sa_handler = master_interrupt_handle;
    sigaction(SIGINT, &sa, NULL);

#ifdef VERBOSE
    printf("-- Sig handler done\n");
#endif

    makeArguments(argvSpawns, IPC_array);

#ifdef VERBOSE
    printf("-- Made arguments\n");
#endif

    for (uCounter = 0; uCounter < par->SO_USER_NUM; uCounter++)
    {
        usersPID[uCounter].status = alive;
        usersPID[uCounter].pid = spawn_user(argvSpawns);
        if (getpid() != myPID)
        {
            switch (returnVal = wait(NULL))
            {
            case MAX_RETRY:
                /* change status in usersPID */
                printf("User %d has died because of too many retries :(\n", getpid());
                break;
            }

            return;
        }
    }

    for (nCounter = 0; nCounter < par->SO_NODES_NUM; nCounter++)
    {
        nodesPID[nCounter].status = available;
        nodesPID[nCounter].pid = spawn_node(argvSpawns);
        if (getpid() != myPID)
        {
            return;
        }
    }

    sleep(simTime);

    print_time_to_die();
    /* kill(myPID, SIGINT); /* our sigint handler needs to do quite a lot of things to print the wall of test below */

    { /* -- FINAL PRINT -- */
        print_user_nodes_table(myPID, usersPID, nodesPID, par->SO_USER_NUM, par->SO_NODES_NUM);
        /*print_kill_signal();
        print_user_balance();
        print_node_balance();
        print_num_aborted();
        print_num_blocks();
        print_transactions_still_in_pool();*/
    }

    return 0;
}
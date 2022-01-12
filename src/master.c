#include "include/common.h"
#include "include/master.h"
#include "include/print.h"
#include "include/parser.h"
#include "utils/debug.h"

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
{
    write(1, "::Master:: SIGINT ricevuto\n", 28);
    killpg(0, SIGINT);
    /*
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
    struct timespec simTime_n = {0};
    pid_t myPID = getpid();

    int uCounter, nCounter, returnVal;
    int IPC_array[3] = {0};
    unsigned int simTime;
    char *argvSpawns[10] = {0};

    user *usersPID;
    node *nodesPID;

    /* we two separate handlers for CTRL-C and SIGUSR2, it simpflifies everything */
    struct sigaction sa;

    struct sembuf sops;
    struct parameters *par;

    /* shared memory segments IDs */
    int usersPID_ID;
    int nodesPID_ID;
    int par_ID;

    par_ID = shmget((key_t)SHM_PARAMETERS, sizeof(par), IPC_CREAT | IPC_EXCL | 0600);
    par = (struct parameters *)shmat(par_ID, NULL, 0);
    if (parse_parameters(par) == CONF_ERROR)
        TRACE(("-- Error reading conf file, defaulting to conf#1\n"));
#ifdef DEBUG
    print_parameters(par);
#endif

    usersPID_ID = shmget((key_t)SHM_USERS_ARRAY, (par->SO_USER_NUM) * sizeof(user), IPC_CREAT | IPC_EXCL | 0600);
    nodesPID_ID = shmget((key_t)SHM_NODES_ARRAY, (par->SO_NODES_NUM) * sizeof(node), IPC_CREAT | IPC_EXCL | 0600);
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

    /* -- SIGNAL HANDLERS --
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
    setpgid(0, 0);

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

    simTime_n.tv_sec = par->SO_SIM_SEC;
    nanosleep(&simTime_n, NULL);

    print_time_to_die();
    final_print(myPID, usersPID, nodesPID, par);
    killpg(0, SIGINT); /* our sigint handler needs to do quite a lot of things to print the wall of test below */

    return 0;
}
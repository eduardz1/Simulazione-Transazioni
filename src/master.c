#include "include/common.h"
#include "include/master.h"
#include "include/print.h"
#include "include/parser.h"

#define SHM_NUM 4
#define SEM_NUM 1
#define IPC_NUM 8

#define USER_NAME "./users"
#define NODE_NAME "./nodes"

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

/*extern int usersPrematurelyDead = 0;*/

/*
 ======================
 ||    FUNCTIONS     ||
 ======================
 */

/* make argv array with IPC IDs for user and nodes */
void make_arguments(int *IPC_array, char **argv)
{
    char *uPID_array = malloc(3 * sizeof(IPC_array[0]) + 1);
    char *nPID_array = malloc(3 * sizeof(IPC_array[0]) + 1);
    char *parameters = malloc(3 * sizeof(IPC_array[0]) + 1);
    char *ledger = malloc(3 * sizeof(IPC_array[0]) + 1);
    char *semID = malloc(3 * sizeof(IPC_array[0]) + 1);

    sprintf(uPID_array, "%d", IPC_array[0]);
    sprintf(nPID_array, "%d", IPC_array[1]);
    sprintf(parameters, "%d", IPC_array[2]);
    sprintf(ledger, "%d", IPC_array[3]);
    sprintf(semID, "%d", IPC_array[4]);

    argv[0] = USER_NAME; /* need nodes to have a different name but not a priority */
    argv[1] = uPID_array;
    argv[2] = nPID_array;
    argv[3] = parameters;
    argv[4] = ledger;
    argv[5] = semID;
    argv[8] = NULL; /* Terminating argv with NULL value */
}

/* fork and execve a "./users" */
pid_t spawn_user(char *userArgv[])
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        printf("-- Error forking for user\n");
        break;

    case 0: /* Child case */
        TRACE((":master: Spawning user\n"));
        execve(USER_NAME, userArgv, NULL);
        TEST_ERROR
        TRACE(("!! Message that should never be seen\n"));
        break;

    default:
        return myPID;
    }
}

/* fork and execve a "./nodes" */
pid_t spawn_node(char *nodeArgv[])
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        printf("!! Error forking for node\n");
        break;

    case 0: /* Child case */
        TRACE((":master: Spawning node\n"));
        execve(NODE_NAME, nodeArgv, NULL);
        TEST_ERROR
        TRACE(("!! Message that should never be seen\n"));
        break;

    default:
        return myPID;
    }
}

/* attach usersPID, nodesPID, par and mainLedger to shared memory, returns an array with respective IDs */
void shared_memory_objects_init(int *shmArray)
{
    /* shared memory segments IDs */
    int usersPID_ID;
    int nodesPID_ID;
    int mainLedger_ID;
    int par_ID;

    /* parameters init and read from conf file */
    par_ID = shmget(SHM_PARAMETERS, sizeof(par), 0600 | IPC_CREAT | IPC_EXCL);
    TEST_ERROR
    par = (struct parameters *)shmat(par_ID, NULL, 0);
    if (parse_parameters(par) == CONF_ERROR)
    {
        TRACE(("-- Error reading conf file, defaulting to conf#1\n"));
    }
    else
    {
        TRACE(("-- Conf file read successful\n"));
    }
#ifdef DEBUG
    print_parameters(par);
#endif
    /* (users_t) and (nodes_t) arrays */
    usersPID_ID = shmget(SHM_USERS_ARRAY,
                         (par->SO_USER_NUM) * sizeof(user),
                         0600 | IPC_CREAT | IPC_EXCL);
    TEST_ERROR
    nodesPID_ID = shmget(SHM_NODES_ARRAY,
                         (par->SO_NODES_NUM) * sizeof(node),
                         0600 | IPC_CREAT | IPC_EXCL);
    TEST_ERROR
    usersPID = (user *)shmat(usersPID_ID, NULL, 0);
    nodesPID = (node *)shmat(nodesPID_ID, NULL, 0);

    /* mainLedger */
    mainLedger_ID = shmget(SHM_LEDGER,
                           (par->SO_NODES_NUM) * sizeof(node),
                           0600 | IPC_CREAT | IPC_EXCL);
    TEST_ERROR
    mainLedger = (ledger *)shmat(mainLedger_ID, NULL, 0);

    /* mark for deallocation so that they are automatically
     * removed once master dies
     */
    shmctl(usersPID_ID, IPC_RMID, NULL);
    shmctl(nodesPID_ID, IPC_RMID, NULL);
    shmctl(par_ID, IPC_RMID, NULL);
    shmctl(mainLedger_ID, IPC_RMID, NULL);

    shmArray[0] = usersPID_ID;
    shmArray[1] = nodesPID_ID;
    shmArray[2] = par_ID;
    shmArray[3] = mainLedger_ID;
}

void semaphores_init()
{
    semID = semget(SEM_MASTER, 1, 0600 | IPC_CREAT | IPC_EXCL);
    TEST_ERROR
    TRACE((":master: semID is %d\n", semID));
}

/* makes an array with every IPC object ID */
void make_ipc_array(int *IPC_array)
{
    int shmIDs[SHM_NUM]; /* array containing every shared memory ID */
    int semIDs[1] = {0};

    shared_memory_objects_init(shmIDs);
    semIDs[0] = semID;
    /* semaphores_init(semIDs); */
    memcpy(IPC_array, shmIDs, SHM_NUM * sizeof(int));
    memcpy(IPC_array + SHM_NUM, shmIDs, SEM_NUM * sizeof(int));
}

/* CTRL-C handler */
void master_interrupt_handle(int signum)
{
    write(1, "::Master:: SIGINT ricevuto\n", 28);
    killpg(0, SIGINT);

    /* just to avoid printing before everyone has finished*/
    sleep(1);
    final_print(getpid(), usersPID, nodesPID, par);

    /*
    int status;

    while (wait(&status) != -1)
    {
        status >> 8; /* no idea about what it does please help *
    } 
    */

    semctl(semID, 1, IPC_RMID);
    exit(0);
}

int main(int argc, char *argv[])
{
    pid_t myPID = getpid();

    int uCounter, nCounter, returnVal;
    int simTime;
    int ipcObjectsIDs[IPC_NUM];
    char *argvSpawns[8] = {0};

    struct sigaction sa;
    struct sembuf sops;

    semaphores_init();
    make_ipc_array(ipcObjectsIDs);
    make_arguments(ipcObjectsIDs, argvSpawns);
    mainLedger = ledger_init();

    simTime = par->SO_SIM_SEC;

    /* -- SIGNAL HANDLER --
     * first set all bytes of sigation to 0
     * then initialize sa.handler to a pointer to the function interrupt_handle
     * then set the handler to handle SIGINT signals ((struct sigaction *oldact) = NULL)
     */
    bzero(&sa, sizeof(sa));
    sa.sa_handler = master_interrupt_handle;
    sigaction(SIGINT, &sa, NULL);

    for (nCounter = 0; nCounter < par->SO_NODES_NUM; nCounter++)
    {
        LOCK
            nodesPID[nCounter]
                .status = available;
        nodesPID[nCounter].pid = spawn_node(argvSpawns);
        UNLOCK
        if (getpid() != myPID)
        {
            return;
        }
    }

    /*usersPrematurelyDead = 0;*/
    for (uCounter = 0; uCounter < par->SO_USER_NUM; uCounter++)
    {
        LOCK
            usersPID[uCounter]
                .status = alive;
        usersPID[uCounter].pid = spawn_user(argvSpawns);
        UNLOCK
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

    sleep(simTime);

    print_time_to_die();
    killpg(0, SIGINT); /* our sigint handler needs to do quite a lot of things to print the wall of test below */

    return 0;
}
#include "include/common.h"
#include "include/master.h"
#include "include/print.h"
#include "include/parser.h"

#define SHM_NUM 4
#define SEM_NUM 2
#define IPC_NUM SHM_NUM+SEM_NUM

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

int semPIDs_ID;
int semLedger_ID;

/*extern int usersPrematurelyDead = 0;*/

/*
 ======================
 ||    FUNCTIONS     ||
 ======================
 */

/* make argv array with IPC IDs for user and nodes, mode 0 user, mode 1 nodes*/
void make_arguments(int *IPC_array, char **argv)
{
    char *uPID_array = malloc(3 * sizeof(IPC_array[0]) + 1);
    char *nPID_array = malloc(3 * sizeof(IPC_array[0]) + 1);
    char *parameters = malloc(3 * sizeof(IPC_array[0]) + 1);
    char *ledger = malloc(3 * sizeof(IPC_array[0]) + 1);
    char *semPIDs_ID = malloc(3 * sizeof(IPC_array[0]) + 1);
    char *semLedger_ID = malloc(3 * sizeof(IPC_array[0]) + 1);

    sprintf(uPID_array, "%d", IPC_array[0]);
    sprintf(nPID_array, "%d", IPC_array[1]);
    sprintf(parameters, "%d", IPC_array[2]);
    sprintf(ledger, "%d", IPC_array[3]);
    sprintf(semPIDs_ID, "%d", IPC_array[4]);
    sprintf(semLedger_ID, "%d", IPC_array[5]);

    argv[1] = uPID_array;
    TRACE(("[MASTER] argv[uPID] = %s\n", uPID_array))
    argv[2] = nPID_array;
    TRACE(("[MASTER] argv[nPID] = %s\n", nPID_array))
    argv[3] = parameters;
    TRACE(("[MASTER] argv[par] = %s\n", parameters))
    argv[4] = ledger;
    TRACE(("[MASTER] argv[ledger] = %s\n", ledger))
    argv[5] = semPIDs_ID;
    TRACE(("[MASTER] argv[sem_pids] = %s\n", semPIDs_ID))
    argv[6] = semLedger_ID;
    TRACE(("[MASTER] argv[sem_ledger] = %s\n", argv[6]))
    argv[8] = NULL; /* Terminating argv with NULL value */
}

/* fork and execve a "./users" */
pid_t spawn_user(char *userArgv[])
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        printf("*** Error forking for user ***\n");
        break;

    case 0: /* Child case */
        TRACE(("[MASTER] Spawning user\n"));
        execve(USER_NAME, userArgv, NULL);
        TEST_ERROR
        TRACE(("!!! Message that should never be seen !!!\n"));
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


ledger *ledger_init()
{
    ledger *newLedger;
    int shmID; /* ID of "ledger" shared memory segment */

    /* -- LEDGER INITIALIZATION --
     * save the ID of our new (IPC_PRIVATE) shared memory segment of size -ledger-
     * smctl will deallocate the shared memory segment only when every process detaches it
     * tells OS that ledger of type ledger is our shared memory of shmID
     */
    shmID = shmget(IPC_PRIVATE, sizeof(newLedger), 0600);
    shmctl(shmID, IPC_RMID, NULL);

    newLedger->head = NULL;
    newLedger->registryCurrSize = 1;

    newLedger = (ledger *)shmat(shmID, NULL, 0);

    return newLedger;
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
     * this will set the key to 0x00000000
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
    semPIDs_ID = semget(SEM_PIDS_KEY, 1, 0600 | IPC_CREAT | IPC_EXCL);
    TEST_ERROR
    TRACE(("[MASTER] semPIDs_ID is %d\n", semPIDs_ID));

    semLedger_ID = semget(SEM_LEDGER_KEY, 1, 0600 | IPC_CREAT | IPC_EXCL);
    TEST_ERROR
    TRACE(("[MASTER] semLedger_ID is %d\n", semLedger_ID));
}

/* makes an array with every IPC object ID */
void make_ipc_array(int *IPC_array)
{
    int shmIDs[SHM_NUM]; /* array containing every shared memory ID */
    int semIDs[SEM_NUM];

    shared_memory_objects_init(shmIDs);
    semIDs[0] = semPIDs_ID;
    semIDs[1] = semLedger_ID;
    /* semaphores_init(semIDs); */
    memcpy(IPC_array, shmIDs, SHM_NUM * sizeof(int));
    memcpy(IPC_array + SHM_NUM, semIDs, SEM_NUM * sizeof(int));
    TRACE(("[MASTER] IPC_array={%d,%d,%d,%d,%d,%d}\n", IPC_array[0], IPC_array[1], IPC_array[2], IPC_array[3], IPC_array[4], IPC_array[5]))
}

/* CTRL-C handler */
void master_interrupt_handle(int signum)
{
    write(1, "::MASTER:: SIGINT ricevuto\n", 28);
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

    semctl(semPIDs_ID, 1, IPC_RMID);
    semctl(semLedger_ID, 1, IPC_RMID);
    exit(0);
}

int main(int argc, char *argv[])
{
    pid_t myPID = getpid();

    int uCounter, nCounter, returnVal;
    int simTime;
    int ipcObjectsIDs[IPC_NUM];
    char *argvSpawns[9 * (3 * sizeof(int) + 1)] = {0};

    struct sigaction sa;
    struct sembuf sops;

    semaphores_init();
    make_ipc_array(ipcObjectsIDs);
    /*TRACE(("1ARGV[6] is %s\n", argvSpawns[6]))*/
    make_arguments(ipcObjectsIDs, argvSpawns);
    /*TRACE(("2ARGV[6] is %s\n", argvSpawns[6]))*/
    /*mainLedger = ledger_init();
    /*TRACE(("3ARGV[6] is %s\n", argvSpawns[6]))*/
    simTime = par->SO_SIM_SEC;

    /* -- SIGNAL HANDLER --
     * first set all bytes of sigation to 0
     * then initialize sa.handler to a pointer to the function interrupt_handle
     * then set the handler to handle SIGINT signals ((struct sigaction *oldact) = NULL)
     */
    bzero(&sa, sizeof(sa));
    sa.sa_handler = master_interrupt_handle;
    sigaction(SIGINT, &sa, NULL);

    argvSpawns[0] = NODE_NAME;
    TRACE(("4ARGV[6] is %d\n", argvSpawns[6]))
    TRACE(("[MASTER] argv values for nodes: %s %s %s %s %s %s %s %s %s\n", argvSpawns[0], argvSpawns[1], argvSpawns[2], argvSpawns[3], argvSpawns[4], argvSpawns[5], argvSpawns[6], argvSpawns[7], argvSpawns[8]))
    for (nCounter = 0; nCounter < par->SO_NODES_NUM; nCounter++)
    {
        LOCK
            nodesPID[nCounter]
                .status = available;
        nodesPID[nCounter].balance = 0;
        nodesPID[nCounter].pid = spawn_node(argvSpawns);
        UNLOCK
        if (getpid() != myPID)
        {
            return;
        }
    }

    /*usersPrematurelyDead = 0;*/
    argvSpawns[0] = USER_NAME;
    TRACE(("ARGV[6] is %d\n", argvSpawns[6]))
    TRACE(("[MASTER] argv values for users: %s %s %s %s %s %s %s %s %s\n", argvSpawns[0], argvSpawns[1], argvSpawns[2], argvSpawns[3], argvSpawns[4], argvSpawns[5], argvSpawns[6], argvSpawns[7], argvSpawns[8]))
    for (uCounter = 0; uCounter < par->SO_USER_NUM; uCounter++)
    {
        LOCK
            usersPID[uCounter]
                .status = alive;
        usersPID[uCounter].balance = 0;
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
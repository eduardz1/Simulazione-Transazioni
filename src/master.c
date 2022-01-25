#include "include/common.h"
#include "include/master.h"
#include "include/print.h"
#include "include/parser.h"

#define SHM_NUM 4
#define SEM_NUM 2
#define IPC_NUM SHM_NUM + SEM_NUM

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
block ledger[SO_REGISTRY_SIZE];
block *ledger_ptr;

int semPIDs_ID;
int semLedger_ID;
int masterQ;

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

/* initializes message queue specific to own PID */
int message_queue_init()
{
    int ownPID = getpid();
    /* gets ID of message queue of key=myPID and assigns it to queueID */
    int queueID = msgget(ownPID, IPC_CREAT | IPC_EXCL | 0600);
    switch (errno)
    {
    case EIDRM:
        printf("[PROCESS %d] queue %d was removed\n", ownPID, queueID);
        break;
    case EINVAL:
        printf("[PROCESS %d] queue %d invalid value for cmd or msqid\n", ownPID, queueID);
        break;
    case EPERM:
        printf("[PROCESS %d] queue %d the effective user ID of the calling process is not the creator or the owner\n", ownPID, queueID);
        break;
    }
    TRACE(("[PROCESS %d] queueID is %d\n", ownPID, queueID))
    return queueID;
}

/* fork and execve a "./users" */
void spawn_user(char *userArgv[], int uCounter)
{
    pid_t userPID = fork();
    switch (userPID)
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
        usersPID[uCounter].pid = userPID;
        return;
    }
}

/* fork and execve a "./nodes" */
int spawn_node(char *nodeArgv[], int nCounter)
{
    static int overBuf = 0;
    struct msgbuf_friends friendsMsg;
    pid_t nodePID = fork();
    pid_t *friends = malloc(sizeof(pid_t) * par->SO_FRIENDS_NUM);
    make_friend_list(friends);

    friendsMsg.mtype = FRIENDS_MTYPE;
    friendsMsg.friendList = friends;

    if (overBuf == 0)
        overBuf = par->SO_NODES_NUM;
    if (overBuf >= par->SO_NODES_NUM * 2 - 1) /* maximum capacity reached */
        return ERROR;
    else if (nCounter == -1)
        nCounter = overBuf++;

    switch (nodePID)
    {
    case -1: /* Error case */
        printf("*** Error forking for node ***\n");
        break;

    case 0: /* Child case */
        TRACE(("[MASTER] Spawning node with associated queue\n"));

        /* initialize a separate message queue for every node */

        send_message(message_queue_init(), &friendsMsg, sizeof(struct msgbuf_friends), 0);

        execve(NODE_NAME, nodeArgv, NULL);
        TEST_ERROR
        TRACE(("!!! Message that should never be seen !!!\n"));
        break;

    default:
        nodesPID[nCounter].pid = nodePID;
        break;
    }
    return SUCCESS;
}

/* generates a friend list based on Knuth algorithm */
void make_friend_list(pid_t *friends)
{
    /* the problem is that with num friends close to num nodes extracting
     * the last few friends becomes increasingly less likely to succedeed;
     * Using the knoth algoritm we generate an array of random indexes
     * in ascending order, that's not a problem because the nodes will extract
     * a random index anyway when "hopping" a transaction to a different node
     */

    /* Knuth algorithm */ /*friends = m, nodes = n */
    const int numNodes = par->SO_NODES_NUM;
    const int numFriends = par->SO_FRIENDS_NUM;

    int iFriends = 0, iNodes;

    int toIterate, toFind;

    int *friendsIndexes = malloc(sizeof(int) * numNodes);
    int i;

    srand(time(NULL));
    for (iNodes = 0; iFriends < numFriends && iNodes < numNodes; ++iNodes)
    {
        toIterate = numNodes - iNodes;
        toFind = numFriends - iFriends;
        if (rand() % toIterate < toFind)
            friendsIndexes[iFriends++] = iNodes + 1; /* +1 since your range begins from 1 */
    }

    for (i = 0; i < numFriends; i++)
    {
        friends[i] = nodesPID[friendsIndexes[i]].pid;
    }

    free(friendsIndexes);
}

/* attach usersPID, nodesPID, par and mainLedger to shared memory, returns an array with respective IDs */
void shared_memory_objects_init(int *shmArray)
{
    /* shared memory segments IDs */
    int usersPID_ID;
    int nodesPID_ID;
    int ledger_ID;
    int par_ID;

    /* parameters init and read from conf file */
    par_ID = shmget(SHM_PARAMETERS, sizeof(par), 0600 | IPC_CREAT | IPC_EXCL);
    TEST_ERROR
    par = (struct parameters *)shmat(par_ID, NULL, 0);
    if (parse_parameters(par) == CONF_ERROR)
    {
        TRACE(("*** Error reading conf file, defaulting to conf#1 ***\n"));
    }
    else
    {
        TRACE(("[MASTER] conf file read successful\n"));
    }
#ifdef DEBUG
    print_parameters(par);
#endif
    /* (users_t) and (nodes_t) arrays */
    usersPID_ID = shmget(SHM_USERS_ARRAY,
                         (par->SO_USER_NUM) * sizeof(user),
                         0600 | IPC_CREAT | IPC_EXCL);
    TEST_ERROR
    /* make it twice as big to account for extra nodes and avoid reallocating
     * a shared memory resource
     */
    nodesPID_ID = shmget(SHM_NODES_ARRAY,
                         (par->SO_NODES_NUM) * sizeof(node) * 2,
                         0600 | IPC_CREAT | IPC_EXCL);
    TEST_ERROR
    usersPID = (user *)shmat(usersPID_ID, NULL, 0);
    nodesPID = (node *)shmat(nodesPID_ID, NULL, 0);

    /* ledger */
    ledger_ID = shmget(SHM_LEDGER, sizeof(ledger), 0600 | IPC_CREAT | IPC_EXCL);
    TEST_ERROR
    ledger_ptr = (block *)shmat(ledger_ID, NULL, 0);

    /* mark for deallocation so that they are automatically
     * removed once master dies
     * this will set the key to 0x00000000
     */
    shmctl(usersPID_ID, IPC_RMID, NULL);
    shmctl(nodesPID_ID, IPC_RMID, NULL);
    shmctl(par_ID, IPC_RMID, NULL);
    shmctl(ledger_ID, IPC_RMID, NULL);

    shmArray[0] = usersPID_ID;
    shmArray[1] = nodesPID_ID;
    shmArray[2] = par_ID;
    shmArray[3] = ledger_ID;
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

void start_continuous_print()
{
    int i, activeUsers, activeNodes;
    int time = par->SO_SIM_SEC;

    while (time)
    {
        activeNodes = 0;
        activeUsers = 0;

        for (i = 0; i < par->SO_USER_NUM; i++)
        {
            if (usersPID[i].status == alive)
                activeUsers++;
        }

        for (i = 0; i < par->SO_NODES_NUM; i++)
        {
            if (nodesPID[i].status == available)
                activeNodes++;
        }

        /*printf("\r\nNUM ACTIVE USERS: %d\nNUM ACTIVE NODES: %d\n\n", activeUsers, activeNodes);
        fflush(stdout);*/

        sleep(time--);
    }

    printf("\r\nNUM ACTIVE USERS: %d\nNUM ACTIVE NODES: %d\n\n", activeUsers, activeNodes);
}

/* CTRL-C handler */
void master_interrupt_handle(int signum)
{
    int status, wpid;

    write(1, "::MASTER:: SIGINT ricevuto\n", 28);
    killpg(0, SIGINT);

    /* just to avoid printing before everyone has finished*/
    /*while (wait(&status) != -1)

    {
        status >> 8; /* no idea about what it does please help *
    }*/

    while ((wpid = wait(&status)) > 0)
        ;
    final_print(getpid(), usersPID, nodesPID, par);

    print_ledger(ledger_ptr);

    semctl(semPIDs_ID, 1, IPC_RMID);
    semctl(semLedger_ID, 1, IPC_RMID);
    msgctl(masterQ, IPC_RMID, NULL);

    exit(0);
}

int main(int argc, char *argv[])
{
    pid_t myPID = getpid();

    int uCounter, nCounter, returnVal;
    int ipcObjectsIDs[IPC_NUM];
    char *argvSpawns[9 * (3 * sizeof(int) + 1)] = {0};

    struct sigaction sa;
    struct sembuf sops;

    ledger_ptr = ledger;
    semaphores_init();
    make_ipc_array(ipcObjectsIDs);
    make_arguments(ipcObjectsIDs, argvSpawns);

    /* -- SIGNAL HANDLER --
     * first set all bytes of sigation to 0
     * then initialize sa.handler to a pointer to the function interrupt_handle
     * then set the handler to handle SIGINT signals ((struct sigaction *oldact) = NULL)
     */
    bzero(&sa, sizeof(sa));
    sa.sa_handler = master_interrupt_handle;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGALRM, &sa, NULL);

    /* init message queue to receive transactions that have hopped too much */
    masterQ = message_queue_init();

    argvSpawns[0] = NODE_NAME;
    TRACE(("[MASTER] argv values for nodes: %s %s %s %s %s %s %s %s %s\n", argvSpawns[0], argvSpawns[1], argvSpawns[2], argvSpawns[3], argvSpawns[4], argvSpawns[5], argvSpawns[6], argvSpawns[7], argvSpawns[8]))
    for (nCounter = 0; nCounter < par->SO_NODES_NUM; nCounter++)
    {
        LOCK;
        nodesPID[nCounter].status = available;
        nodesPID[nCounter].balance = 0;
        spawn_node(argvSpawns, nCounter);
        UNLOCK
        if (getpid() != myPID)
        {
            return 0;
        }
    }

    /*usersPrematurelyDead = 0;*/
    argvSpawns[0] = USER_NAME;
    TRACE(("[MASTER] argv values for users: %s %s %s %s %s %s %s %s %s\n", argvSpawns[0], argvSpawns[1], argvSpawns[2], argvSpawns[3], argvSpawns[4], argvSpawns[5], argvSpawns[6], argvSpawns[7], argvSpawns[8]))
    for (uCounter = 0; uCounter < par->SO_USER_NUM; uCounter++)
    {
        LOCK;
        usersPID[uCounter].status = alive;
        usersPID[uCounter].balance = 0;
        spawn_user(argvSpawns, uCounter);
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

            return 0;
        }
    }

    alarm(par->SO_SIM_SEC);
    switch (fork())
    {
    case -1:
        TRACE(("[MASTER] error forking\n"))
        break;

    case 0:
    {
        struct msgbuf_trans transHopped;
        signal(SIGINT, SIG_DFL); /* else it would print ledger and all twice */
        while (1)
        {
            receive_message(masterQ, &transHopped, sizeof(struct msgbuf_trans), TRANSACTION_MTYPE, 0);
            argvSpawns[0] = NODE_NAME;
            TRACE(("[MASTER] argv values for nodes extra: %s %s %s %s %s %s %s %s %s\n", argvSpawns[0], argvSpawns[1], argvSpawns[2], argvSpawns[3], argvSpawns[4], argvSpawns[5], argvSpawns[6], argvSpawns[7], argvSpawns[8]))
   

            LOCK;
            nodesPID[nCounter].status = available;
            nodesPID[nCounter].balance = 0;
            if (spawn_node(argvSpawns, -1) != 0)
            {
                TRACE(("[MASTER] too many nodes created to compensate for hopping already\n"))
                UNLOCK
                break;
            }
            TRACE(("[MASTER] spawned a node due to hopped transaction\n"))
            UNLOCK
            if (getpid() != myPID)
                return 0;

            /* missing section where master orders to existing nodes to add this one to
             * their list of friends
             */
        }
    }
    break;

    default:
        start_continuous_print();

        killpg(0, SIGINT); /* our sigint handler needs to do quite a lot of things to print the wall of test below */
        return 0;
    }
    return 0;
}
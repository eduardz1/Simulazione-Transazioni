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

enum term termReason = timeElapsed;

/*
 ======================
 ||    FUNCTIONS     ||
 ======================
 */

/* make argv array with IPC IDs for user and nodes, mode 0 user, mode 1 nodes*/
void make_arguments(int *IPC_array, char **argv)
{
    char uPID_array[13] = {0};
    char nPID_array[13] = {0};
    char parameters[13] = {0};
    char ledger[13] = {0};
    char semPIDs_ID[13] = {0};
    char semLedger_ID[13] = {0};

    snprintf(uPID_array, 13, "%d", IPC_array[0]);
    snprintf(nPID_array, 13, "%d", IPC_array[1]);
    snprintf(parameters, 13, "%d", IPC_array[2]);
    snprintf(ledger, 13, "%d", IPC_array[3]);
    snprintf(semPIDs_ID, 13, "%d", IPC_array[4]);
    snprintf(semLedger_ID, 13, "%d", IPC_array[5]);

    strncpy(argv[1], uPID_array, 13);
    strncpy(argv[2], nPID_array, 13);
    strncpy(argv[3], parameters, 13);
    strncpy(argv[4], ledger, 13);
    strncpy(argv[5], semPIDs_ID, 13);
    strncpy(argv[6], semLedger_ID, 13);
    argv[7] = NULL; /* Terminating argv with NULL value */

    TRACE(("[MASTER] argv[uPID] = %s\n", argv[1]))
    TRACE(("[MASTER] argv[nPID] = %s\n", argv2))
    TRACE(("[MASTER] argv[par] = %s\n", argv[3]))
    TRACE(("[MASTER] argv[ledger] = %s\n", argv[4]))
    TRACE(("[MASTER] argv[sem_pids] = %s\n", argv[5]))
    TRACE(("[MASTER] argv[sem_ledger] = %s\n", argv[6]))
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
        TRACE(("[MASTER] Spawning user\n"))
        execve(USER_NAME, userArgv, NULL);
        TEST_ERROR
        TRACE(("!!! Message that should never be seen !!!\n"))
        break;

    default:
        usersPID[uCounter].pid = userPID;
        return;
    }
}

/* generates a friend list based on Knuth algorithm */
void make_friend_list(pid_t *friends)
{
    /* the problem is that with num friends close to num nodes extracting
     * the last few friends becomes increasingly less likely to succedeed;
     * Using the knoth algoritm we generate an array of random indexes
     * in ascending order, that's not a problem because the nodes will extract
     * a random index anyway when "hopping" a transaction to a different node
     * idea from https://stackoverflow.com/questions/1608181/unique-random-number-generation-in-an-integer-array
     */

    /* Knuth algorithm */
    const int numNodes = par->SO_NODES_NUM;     /* 2 */
    const int numFriends = par->SO_FRIENDS_NUM; /* 1 */

    int iFriends = 0, iNodes;

    int toIterate, toFind;

    int *friendsIndexes = malloc(sizeof(int) * numNodes);
    int i;

    srand(time(NULL));
    for (iNodes = 0; iFriends < numFriends && iNodes < numNodes; ++iNodes)
    {
        toIterate = numNodes - iNodes;   /* 2-1 == 1 */
        toFind = numFriends - iFriends;  /* 1-0 == 1*/
        if (rand() % toIterate < toFind) /* iterate either 1 or 0 */
            friendsIndexes[iFriends++] = iNodes;
    }

    for (i = 0; i < numFriends; i++)
    {
        friends[i] = nodesPID[friendsIndexes[i]].pid;
        TRACE(("[MASTER] extracted friend: %d\n", friends[i]))
    }

    free(friendsIndexes);
}

void send_friend_list(pid_t node)
{
    int nodeQueue, i;
    pid_t *friends = malloc(sizeof(pid_t) * par->SO_FRIENDS_NUM);
    struct msgbuf_friends friendsMsg;

    make_friend_list(friends);
    TRACE(("[MASTER] please have this be a friend that makes sense: %d\n", friends[0]))
    do
    {
        errno = 0;
        TRACE(("[MASTER] trying msgget for nodesPID[x].%d\n", node))
        nodeQueue = msgget(node, 0);
    } while (errno == ENOENT);

    TRACE(("[MASTER] nodePID.pid=%d queue=%d\n", node, nodeQueue))

    for (i = 0; i < par->SO_FRIENDS_NUM; i++)
    {
        friendsMsg.mtype = FRIENDS_MTYPE;
        friendsMsg.friend = friends[i];

        send_message(nodeQueue, &friendsMsg, sizeof(struct msgbuf_friends), 0);
        TRACE(("[MASTER] sent friend to %d\n", node))
    }

    free(friends);
}

/* fork and execve a "./nodes" */
int spawn_node(char *nodeArgv[], int nCounter)
{
    static int overBuf = 0;
    pid_t nodePID = fork();

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
        TRACE(("[MASTER] Spawning node with associated queue\n"))

        /* initialize a separate message queue for every node */

        message_queue_init();

        execve(NODE_NAME, nodeArgv, NULL);
        TEST_ERROR
        TRACE(("!!! Message that should never be seen !!!\n"))
        break;

    default:
        TRACE(("[MASTER] filling nodesPID\n"))
        nodesPID[nCounter].pid = nodePID;
        /*sem_reserve(semFriends_ID, 1);*/
        break;
    }
    return nodePID;
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
    par = (struct parameters *)shmat(par_ID, NULL, 0);
    if (parse_parameters(par) == CONF_ERROR)
    {
        TRACE(("*** Error reading conf file, defaulting to conf#1 ***\n"))
    }
    else
    {
        TRACE(("[MASTER] conf file read successful\n"))
    }

    /* make node array twice as big to account for extra nodes when transactions
     * have hopped out avoiding reallocating a shared memory resource
     */
    nodesPID_ID = shmget(SHM_NODES_ARRAY, (par->SO_NODES_NUM) * sizeof(node) * 2, 0600 | IPC_CREAT | IPC_EXCL);
    usersPID_ID = shmget(SHM_USERS_ARRAY, (par->SO_USER_NUM) * sizeof(user), 0600 | IPC_CREAT | IPC_EXCL);
    ledger_ID = shmget(SHM_LEDGER, sizeof(ledger), 0600 | IPC_CREAT | IPC_EXCL);
    if (errno == EEXIST)
    {
        TRACE(("[MASTER] failed to initialize one or more shared memory objects, ipcrm the leftovers\n"))
        exit(1); /* we could remove them at the start just in case, but I want the error not to be hidden */
    }

    ledger_ptr = (block *)shmat(ledger_ID, NULL, 0);
    usersPID = (user *)shmat(usersPID_ID, NULL, 0);
    nodesPID = (node *)shmat(nodesPID_ID, NULL, 0);

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
    semLedger_ID = semget(SEM_LEDGER_KEY, 1, 0600 | IPC_CREAT | IPC_EXCL);

    /* check for only one of them is enough */
    switch (errno)
    {
    case EEXIST:
        TRACE(("[MASTER] one or more semaphores couldn't be created, ipcrm the leftovers\n"))
        exit(1);
        break;

    case ENOSPC:
        TRACE(("[MASTER] too many semaphores already in the system\n"))
        exit(1);
        break;

    default:
        TRACE(("[MASTER] semPIDs_ID is %d\n", semPIDs_ID))
        TRACE(("[MASTER] semLedger_ID is %d\n", semLedger_ID))
        break;
    }
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
    int i, activeUsers, deadUsers, activeNodes;
    int time = par->SO_SIM_SEC;

    while (time--)
    {
        activeNodes = 0;
        activeUsers = 0;
        deadUsers = 0;

        for (i = 0; i < par->SO_USER_NUM; i++)
        {
            if (usersPID[i].status == alive)
                activeUsers++;

            if (usersPID[i].status == dead)
                deadUsers++;
        }

        if (deadUsers >= par->SO_USER_NUM - 1)
        {
            termReason = usersDead;
            killpg(0, SIGINT);
        }

        for (i = 0; i < par->SO_NODES_NUM; i++)
        {
            if (nodesPID[i].status == available)
                activeNodes++;
        }

        printf("\nNUM ACTIVE USERS: %d     \nNUM ACTIVE NODES: %d     \n\n", activeUsers, activeNodes);
        print_most_significant_processes(usersPID, nodesPID, par);

        printf("\033[22A\r"); /*ESC[#A moves cursor up # lines, \r moves cursor to begging of the line */
        sleep(1);
    }
}

/* CTRL-C handler */
void master_interrupt_handle(int signum)
{
    int status, wpid;

    write(2, "::MASTER:: SIGINT ricevuto\n", 28);
    killpg(0, SIGINT);

    /* just to avoid printing before everyone has finished*/
    while ((wpid = wait(&status)) > 0)
        ;

    print_user_nodes_table(getpid(), usersPID, nodesPID, par);
    print_num_aborted(usersPID, par);
    if (print_num_blocks(ledger_ptr) == SO_REGISTRY_SIZE - 1)
    {
        termReason = ledgerFull;
    }
    print_kill_signal(termReason);
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
    char *argvSpawns[8];

    struct sigaction sa;
    struct sembuf sops;

    int i;
    for (i = 0; i < 8; i++)
        argvSpawns[i] = malloc(3 * sizeof(int) + 1);

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
    TRACE(("[MASTER] argv values for nodes: %s %s %s %s %s %s %s %s %s\n", argvSpawns[0], argvSpawns[1], argvSpawns[2], argvSpawns[3], argvSpawns[4], argvSpawns[5], argvSpawns[6], argvSpawns[7]))
    for (nCounter = 0; nCounter < par->SO_NODES_NUM; nCounter++)
    {
        LOCK;
        nodesPID[nCounter].status = available;
        nodesPID[nCounter].balance = 0;
        spawn_node(argvSpawns, nCounter);
        UNLOCK
        if (getpid() != myPID)
            exit(0);
        TRACE(("[MASTER SPAWN %d] timestamp=%lu\n", getpid(), time(NULL)))
    }

    for (nCounter = 0; nCounter < par->SO_NODES_NUM; nCounter++)
    {
        send_friend_list(nodesPID[nCounter].pid);
    }

    argvSpawns[0] = USER_NAME;
    TRACE(("[MASTER] argv values for users: %s %s %s %s %s %s %s %s %s\n", argvSpawns[0], argvSpawns[1], argvSpawns[2], argvSpawns[3], argvSpawns[4], argvSpawns[5], argvSpawns[6], argvSpawns[7]))
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
        int i, tempPID;
        pid_t *privateFriends = malloc(par->SO_FRIENDS_NUM);
        struct msgbuf_friends newNode;
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
            tempPID = spawn_node(argvSpawns, -1);
            UNLOCK
            switch (tempPID)
            {
            case -1:
                TRACE(("[MASTER] too many nodes created to compensate for hopping already\n"))
                free(privateFriends);
                exit(0);
                break;

            default:
                send_friend_list(tempPID);
                TRACE(("[MASTER] spawned a node due to hopped transaction\n"))
                make_friend_list(privateFriends);
                newNode.friend = nodesPID[nCounter].pid;
                newNode.mtype = FRIENDS_MTYPE;
                for (i = 0; i < par->SO_FRIENDS_NUM; i++)
                {
                    send_message(msgget(privateFriends[i], 0), &newNode, sizeof(struct msgbuf_friends), 0);
                }
            }
        }
    }
    break;

    default:
        start_continuous_print();
        break;
    }
    return 0;
}
#include "include/master.h"
#include "include/common.h"
#include "include/parser.h"
#include "include/print.h"
#include "utils/debug.h"
#include <stdbool.h>
#include <stdlib.h>

#define USER_NAME "bin/users"
#define NODE_NAME "bin/nodes"

struct Params *par; /* parameters of simulation */

User *users;
Node *nodes;
Ledger *ledger;

int semUsersPIDs_ID;
int semNodesPIDs_ID;
int semLedger_ID;

int shUsersPIDs_ID;
int shNodesPIDs_ID;
int shLedger_ID;
int shParams_ID;

enum Term termReason = TIME_ELAPSED;
pthread_t ptid; /* continous print thread id */

/* make argv array with IPC IDs for user and nodes, mode 0 user, mode 1 nodes*/
void make_arguments(int *IPC_array, char **argv)
{
    char uPID_array[13] = {0};
    char nPID_array[13] = {0};
    char parameters[13] = {0};
    char ledger_[13] = {0};
    char semUsersPIDs_ID_[13] = {0};
    char semNodesPIDs_ID_[13] = {0};
    char semLedger_ID_[13] = {0};

    snprintf(uPID_array, 13, "%d", IPC_array[0]);
    snprintf(nPID_array, 13, "%d", IPC_array[1]);
    snprintf(parameters, 13, "%d", IPC_array[2]);
    snprintf(ledger_, 13, "%d", IPC_array[3]);
    snprintf(semUsersPIDs_ID_, 13, "%d", IPC_array[4]);
    snprintf(semNodesPIDs_ID_, 13, "%d", IPC_array[6]);
    snprintf(semLedger_ID_, 13, "%d", IPC_array[5]);

    strncpy(argv[1], uPID_array, 13);
    strncpy(argv[2], nPID_array, 13);
    strncpy(argv[3], parameters, 13);
    strncpy(argv[4], ledger_, 13);
    strncpy(argv[5], semUsersPIDs_ID_, 13);
    strncpy(argv[6], semLedger_ID_, 13);
    strncpy(argv[7], semNodesPIDs_ID_, 13);
    argv[8] = NULL; /* Terminating argv with NULL value */

    TRACE(("[MASTER] argv[uPID] = %s\n", argv[1]))
    TRACE(("[MASTER] argv[nPID] = %s\n", argv[2]))
    TRACE(("[MASTER] argv[par] = %s\n", argv[3]))
    TRACE(("[MASTER] argv[ledger] = %s\n", argv[4]))
    TRACE(("[MASTER] argv[sem_users_pids] = %s\n", argv[5]))
    TRACE(("[MASTER] argv[sem_nodes_pids] = %s\n", argv[7]))
    TRACE(("[MASTER] argv[sem_ledger] = %s\n", argv[6]))
}

int message_queue_init(void)
{
    errno = 0;

    int ownPID = getpid();
    int queueID = msgget(ownPID, IPC_CREAT | IPC_EXCL | 0600);

    switch (errno)
    {
    case EIDRM:
        TRACE(("[PROCESS %d] queue %d was removed\n", ownPID, queueID));
        break;
    case EINVAL:
        TRACE(("[PROCESS %d] queue %d invalid value for cmd or msqid\n",
               ownPID,
               queueID));
        break;
    case EPERM:
        TRACE(("[PROCESS %d] queue %d the effective user ID of the calling "
               "process is not the creator or the owner\n",
               ownPID,
               queueID));
        break;
    }
    return queueID;
}

void spawn_user(char *userArgv[], size_t uCounter)
{
    users[uCounter].status = alive;
    users[uCounter].balance = 0;

    pid_t userPID = fork();
    switch (userPID)
    {
    case -1: /* Error case */
        TRACE(("*** Error forking for user ***\n"));
        break;

    case 0: /* Child case */
        TRACE(("[MASTER] Spawning user\n"))
        execve(USER_NAME, userArgv, NULL);
        TEST_ERROR
        break;

    default: users[uCounter].pid = userPID; return;
    }
}

/* generates a friend_ list based on Knuth algorithm */
void make_friend_list(pid_t friends[par->SO_FRIENDS_NUM])
{
    /* the problem is that with num friends close to num nodes extracting
     * the last few friends becomes increasingly less likely to succedeed;
     * Using the knoth algoritm we generate an array of random indexes
     * in ascending order, that's not a problem because the nodes will extract
     * a random index anyway when "hopping" a transaction to a different node
     * idea from
     * https://stackoverflow.com/questions/1608181/unique-random-number-generation-in-an-integer-array
     */

    /* Knuth algorithm */
    const uint32_t numNodes = par->SO_NODES_NUM;     /* 2 */
    const uint32_t numFriends = par->SO_FRIENDS_NUM; /* 1 */

    uint32_t toIterate, toFind, iFriends = 0;

    uint32_t *friendsIndexes;
    safe_alloc(friendsIndexes, numNodes, sizeof(uint32_t));

    srand(time(NULL));

    for (size_t iNodes = 0; iFriends < numFriends && iNodes < numNodes;
         ++iNodes)
    {
        toIterate = numNodes - iNodes;   /* 2-1 == 1 */
        toFind = numFriends - iFriends;  /* 1-0 == 1*/
        if (rand() % toIterate < toFind) /* iterate either 1 or 0 */
            friendsIndexes[iFriends++] = iNodes;
    }

    for (size_t i = 0; i < numFriends; i++)
    {
        friends[i] = nodes[friendsIndexes[i]].pid;
        TRACE(("[MASTER] extracted friend_: %d\n", friends[i]))
    }

    free(friendsIndexes);
}

void send_friend_list(pid_t node)
{
    int nodeQueue;
    pid_t *friends = calloc(par->SO_FRIENDS_NUM, sizeof(pid_t));
    if (friends == NULL)
    {
        TRACE(("*** Error allocating memory for friends ***\n"));
        exit(EXIT_FAILURE);
    }

    struct MsgbufFriends friendsMsg = {0};

    make_friend_list(friends);
    do
    {
        errno = 0;
        TRACE(("[MASTER] trying msgget for nodesPID[x].%d\n", node))
        nodeQueue = msgget(node, 0);
    } while (errno == ENOENT);

    for (size_t i = 0; i < par->SO_FRIENDS_NUM; i++)
    {
        friendsMsg.mtype = FRIENDS_MTYPE;
        friendsMsg.friend_ = friends[i];

        if (send_message(nodeQueue, &friendsMsg, sizeof(pid_t), 0) ==
            EXIT_SUCCESS)
        {
            TRACE(
                ("[MASTER] sent friend_ %d to %d\n", friendsMsg.friend_, node))
        }
    }

    free(friends);
}

int spawn_node(char *nodeArgv[], size_t nCounter)
{
    nodes[nCounter].status = available;
    nodes[nCounter].balance = 0;

    pid_t nodePID = fork();
    switch (nodePID)
    {
    case -1: /* Error case */
        TRACE(("*** Error forking for node ***\n"));
        break;

    case 0: /* Child case */
        TRACE(("[MASTER] Spawning node with associated queue\n"))

        /* initialize a separate message queue for every node */
        message_queue_init();

        execve(NODE_NAME, nodeArgv, NULL);
        TEST_ERROR
        break;

    default: nodes[nCounter].pid = nodePID; break;
    }
    return nodePID;
}

void shd_memory_init()
{
    // /* shared memory segments IDs */
    // int usersPID_ID;
    // int nodesPID_ID;
    // int ledger_ID;
    // int par_ID;

    /* parameters init and read from conf file */
    shParams_ID =
        shmget(IPC_PRIVATE, sizeof(struct Params), 0600 | IPC_CREAT | IPC_EXCL);
    par = (struct Params *)shmat(shParams_ID, NULL, 0);
    if (parse_parameters(par) == EXIT_FAILURE)
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
    shNodesPIDs_ID = shmget(IPC_PRIVATE,
                            (par->SO_NODES_NUM) * sizeof(Node) * 2,
                            0600 | IPC_CREAT | IPC_EXCL);
    shUsersPIDs_ID = shmget(IPC_PRIVATE,
                            (par->SO_USER_NUM) * sizeof(User),
                            0600 | IPC_CREAT | IPC_EXCL);
    shLedger_ID =
        shmget(IPC_PRIVATE, sizeof(Ledger), 0600 | IPC_CREAT | IPC_EXCL);
    if (errno == EEXIST)
    {
        TRACE(("[MASTER] failed to initialize one or more shared memory "
               "objects, ipc-rm the leftovers\n"))
        exit(EXIT_FAILURE); /* we could remove them at the start just in case,
                    but I want the error not to be hidden */
    }

    ledger = (Ledger *)shmat(shLedger_ID, NULL, 0);
    users = (User *)shmat(shUsersPIDs_ID, NULL, 0);
    nodes = (Node *)shmat(shNodesPIDs_ID, NULL, 0);

    /* mark for deallocation so that they are automatically
     * removed once master dies
     */
    shmctl(shUsersPIDs_ID, IPC_RMID, NULL);
    shmctl(shNodesPIDs_ID, IPC_RMID, NULL);
    shmctl(shParams_ID, IPC_RMID, NULL);
    shmctl(shLedger_ID, IPC_RMID, NULL);
}

void semaphores_init(void)
{
    semUsersPIDs_ID = semget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
    semNodesPIDs_ID = semget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
    semLedger_ID = semget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);

    /* check for only one of them is enough */
    switch (errno)
    {
    case EEXIST:
        TRACE(("[MASTER] one or more semaphores couldn't be created, ipcrm the "
               "leftovers\n"))
        exit(EXIT_FAILURE);

    case ENOSPC:
        TRACE(("[MASTER] too many semaphores already in the system\n"))
        exit(EXIT_FAILURE);

    default:
        TRACE(("[MASTER] semUsersPIDs_ID is %d\n", semUsersPIDs_ID))
        TRACE(("[MASTER] semNodesPIDs_ID is %d\n", semNodesPIDs_ID))
        TRACE(("[MASTER] semLedger_ID is %d\n", semLedger_ID))
        break;
    }
}

void make_ipc_array(int *IPC_array)
{
    int ipcIDs[IPC_NUM];

    semaphores_init();
    shd_memory_init();

    ipcIDs[0] = shUsersPIDs_ID;
    ipcIDs[1] = shNodesPIDs_ID;
    ipcIDs[2] = shParams_ID;
    ipcIDs[3] = shLedger_ID;
    ipcIDs[4] = semUsersPIDs_ID;
    ipcIDs[5] = semLedger_ID;
    ipcIDs[6] = semNodesPIDs_ID;

    memcpy(IPC_array, ipcIDs, sizeof(ipcIDs));
}

void *continuous_print_routine()
{
    uint32_t activeUsers, deadUsers, activeNodes;
    uint32_t time = par->SO_SIM_SEC;

    while (time--)
    {
        activeNodes = 0;
        activeUsers = 0;
        deadUsers = 0;

        for (size_t i = 0; i < par->SO_USER_NUM; i++)
        {
            if (users[i].status == alive) activeUsers++;
            if (users[i].status == dead) deadUsers++;
        }

        if (deadUsers >= par->SO_USER_NUM - 1)
        {
            termReason = USER_DEAD;
            killpg(0, SIGINT);
        }

        for (size_t i = 0; i < par->SO_NODES_NUM * 2; i++)
        {
            if (nodes[i].status == available && nodes[i].pid != 0)
                activeNodes++;
        }

        printf("\n[ Time left:  %9d\033[34ms\033[0m ]\n| Active users:%9d |\n| "
               "Active nodes:%9d |\n\n",
               time,
               activeUsers,
               activeNodes);
        print_most_significant_processes(users, nodes, par);

        printf("\033[23A\r"); /*ESC[#A moves cursor up # lines, \r moves cursor
                                 to begging of the line */
        sleep(1);
    }

    return NULL;
}

void handle_extra_nodes(char **argvSpawns, pid_t *privateFriends)
{
    static uint32_t overBuf = 0;
    struct MsgbufFriends newNode = {0};
    struct MsgbufTransData transHopped = {0};

    if (overBuf >= par->SO_NODES_NUM)
        return; /* maximum capacity reached */
    else
        overBuf++;

    receive_message(msgget(getpid(), 0),
                    &transHopped,
                    sizeof(struct MsgbufTransData),
                    TRANSACTIONS_MTYPE,
                    0);

    argvSpawns[0] = NODE_NAME;
    pid_t pid = spawn_node(argvSpawns, par->SO_NODES_NUM + overBuf);

    send_friend_list(pid);
    TRACE(("[MASTER] spawned a node due to hopped transaction\n"))
    make_friend_list(privateFriends);

    newNode.friend_ = nodes[par->SO_NODES_NUM + overBuf].pid;
    newNode.mtype = FRIENDS_MTYPE;

    for (size_t i = 0; i < par->SO_FRIENDS_NUM; send_message(
             msgget(privateFriends[i++], 0), &newNode, sizeof(pid_t), 0))
        ;
}

/* CTRL-C handler */
void master_interrupt_handle(int signum)
{
    int status;
    UNUSED(signum);

    write(2, "::MASTER:: SIGINT ricevuto\n", 28); // FIXME: segfaults
    killpg(0, SIGINT);

    while ((wait(&status)) > 0)
        ;

    // print_user_nodes_table(getpid(), users, nodes, par);
    // print_num_aborted(users, par);
    if (print_num_blocks(ledger) == SO_REGISTRY_SIZE - 1)
        termReason = LEDGER_FULL;

    // print_kill_signal(termReason);
    // print_ledger(ledger);

    semctl(semUsersPIDs_ID, 1, IPC_RMID);
    semctl(semNodesPIDs_ID, 1, IPC_RMID);
    semctl(semLedger_ID, 1, IPC_RMID);
    msgctl(msgget(getpid(), 0), IPC_RMID, NULL);

    exit(EXIT_SUCCESS);
}

int main(void)
{
    int ipcObjectsIDs[IPC_NUM];
    char *argvSpawns[IPC_NUM + 1];

    struct sigaction sa = {0};

    for (size_t i = 0; i < IPC_NUM + 1; i++)
    {
        safe_alloc(argvSpawns[i], 0, (3 * sizeof(int) + 1));
    }

    make_ipc_array(ipcObjectsIDs);
    make_arguments(ipcObjectsIDs, argvSpawns);

    sa.sa_handler = master_interrupt_handle;
    sa.sa_flags = SA_RESETHAND; // to avoid multiple calls
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGALRM, &sa, NULL);

    message_queue_init();

    argvSpawns[0] = NODE_NAME;
    for (size_t i = 0; i < par->SO_NODES_NUM; spawn_node(argvSpawns, i++))
        ;
    for (size_t i = 0; i < par->SO_NODES_NUM; send_friend_list(nodes[i++].pid))
        ; // needs to be done after all nodes are spawned

    argvSpawns[0] = USER_NAME;
    for (size_t i = 0; i < par->SO_USER_NUM; spawn_user(argvSpawns, i++))
        ;

    alarm(par->SO_SIM_SEC);

    pthread_create(&ptid, NULL, continuous_print_routine, NULL);

    for (pid_t privateFriends[par->SO_FRIENDS_NUM];;
         handle_extra_nodes(argvSpawns, privateFriends))
        ;
}

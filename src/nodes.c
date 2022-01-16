#include "include/common.h"
#include "include/nodes.h"

/* transaction pool==transaction's array */

/*
 ======================
 || GLOBAL VARIABLES ||
 ======================
 */

int currBalance;
/* parameters of simulation */
struct parameters *par;
user *usersPID;
node *nodesPID;
ledger *mainLedger;

int semPIDs_ID;
int semLedger_ID;
int queueID;

pid_t myPID;

struct msqid_ds msgType;

/*void Node()
{
    int t_pool[SO_TP_SIZE];
    checkTpFull(t_pool[SO_TP_SIZE]);
    arrayProcesser();
    createBlock();
}

int checkTpFull(int t_pool[SO_TP_SIZE])
{
    if (t_pool[SO_TP_SIZE] == SO_TP_SIZE)
    {
        return 0;
    }
}

void arrayProcesser()
{
    int i = 0;
    for (i; i < SO_TP_SIZE - 1; i++)
    {
    }
}

int createBlock()
{
}

int sleepMethod(int argc, char *argv[])
{
    randSleepTime.tv_sec = 0;
    randSleepTime.tv_nsec = RAND(SO_MIN_TRANS_PROC_NSEC, SO_MAX_TRANS_PROC_NSEC);
}*/

/* sums rewards of sumBlock[SO_BLOCK_SIZE-1] transactions */
int sum_reward(transaction **sumBlock)
{
    int i = 0;
    int sum;

    for (i = 0; i < (SO_BLOCK_SIZE - 1); i++)
    {
        sum += sumBlock[i]->reward;
    }

    currBalance += sum;

    return sum;
}

/* initializes new block with transList[0] as reward transaction */
void new_block(transaction **blockTransaction, block *newBlock)
{
    transaction reward;
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);

    reward.timestamp = timestamp;
    reward.sender = SELF;
    reward.receiver = getpid();
    reward.amount = sum_reward(blockTransaction); /*sum of each reward of transaction in the block */
    reward.reward = 0;

    memcpy(newBlock->transList + 1, *blockTransaction, (SO_BLOCK_SIZE - 1) * (sizeof(transaction)));

    newBlock->next = NULL;
}

/* fills the buffer with SO_BLOCK_SIZE-1 transactions and test for erros */
void fill_block_buffer(transaction **buffer)
{
    int i;
    for (i = 0; i < (SO_BLOCK_SIZE - 1); i++)
    {
        msgrcv(queueID, &buffer[i], sizeof(transaction), 0, 0);
        switch (errno)
        {
        case E2BIG:
            printf("[NODE %d] the message length is greater than sizeof(transaction)\n", myPID);
            break;
        case EACCES:
            printf("[NODE %d] no read permission on queue\n", myPID);
            break;
        case EFAULT:
            printf("[NODE %d] address pointed by msgp inaccessible\n", myPID);
            break;
        case EIDRM:
            printf("[NODE %d] mesage queue removed\n", myPID);
            break;
        case EINTR:
            printf("[NODE %d] signal caught while receiving a message\n", myPID);
            break;
        default:
            TRACE(("[NODE %d] has enough transactions to create a block\n", myPID));
        }
        buffer[i]->status = processing;
    }
}

void append_block_to_ledger(block *newBlock)
{
    ledger *temp = mainLedger;
    int i;

    /* if head is NULL it means that ledger is still empty */
    if (temp->head == NULL)
    {
        mainLedger->head = newBlock;
        mainLedger->registryCurrSize = 1;
        return;
    }

    while (temp->head->next != NULL)
    {
        temp->head = (block *)temp->head->next;
    }

    temp->head->next = (struct block *)newBlock;
    newBlock->prev = (struct block *)temp->head;
    mainLedger->registryCurrSize++;

    for (i = 0; i < SO_BLOCK_SIZE - 1; i++)
    {
        newBlock->transList[i].status = confirmed;
    }
}

/* attaches ipc objects based on IDs passed via arguments */
void attach_ipc_objects(char **argv)
{
    par = shmat(PARAMETERS_ARGV, NULL, 0);
    TRACE(("[NODE %d] par->SO_RETRY %d\n", myPID, par->SO_RETRY))
    TEST_ERROR
    usersPID = shmat(USERS_PID_ARGV, NULL, 0);
    TRACE(("[NODE %d] usersPID[0] = %d, usersPID[3] = %d\n", myPID, usersPID[0], usersPID[3]))
    TEST_ERROR
    nodesPID = shmat(NODES_PID_ARGV, NULL, 0);
    TRACE(("[NODE %d] nodesPID[0] = %d, nodesPID[3] = %d\n", myPID, nodesPID[0], nodesPID[3]))
    TEST_ERROR
    mainLedger = shmat(LEDGER_ARGV, NULL, 0);
    TEST_ERROR
    semPIDs_ID = SEM_PIDS_ARGV;
    TRACE(("[NODE %d] semPIDs_ID is %d\n", myPID, semPIDs_ID));
    semLedger_ID = SEM_LEDGER_ARGV;
    TRACE(("[NODE %d] semLedger_ID is %d\n", myPID, semLedger_ID));
}

/* initializes message queue specific to own PID and sets it's size to SO_TP_SIZE */
void message_queue_init()
{
    /* gets ID of message queue of key=myPID and assigns it to queueID */
    queueID = msgget(myPID, IPC_CREAT | IPC_EXCL | 0600);
    TEST_ERROR

    /* sets size of message queue to be equal to SO_TP_SIZE num of transactions */
    msgType.msg_qbytes = (par->SO_TP_SIZE * sizeof(transaction));
    printf("Message queue size: %ld\n", msgType.msg_qbytes);
    msgType.msg_perm.uid = myPID;
    msgType.msg_perm.gid = getpgid(myPID);
    msgType.msg_perm.mode = 0600;
    msgctl(queueID, IPC_SET, &msgType);
    switch (errno)
    {
    case EIDRM:
        printf("[NODE %d] queue %d was removed\n", myPID, queueID);
        break;
    case EINVAL:
        printf("[NODE %d] queue %d invalid value for cmd or msqid\n", myPID, queueID);
        break;
    case EPERM:
        printf("[NODE %d] queue %d the effective user ID of the calling process is not the creator or the owner\n", myPID, queueID);
        break;
    }
    TRACE(("[NODE %d] queueID is %d\n", myPID, queueID))
}

/* initializes signal handlers for SIGINT */
void signal_handler_init(struct sigaction *saINT_node)
{
    saINT_node->sa_handler = node_interrupt_handle;
    sigaction(SIGINT, saINT_node, NULL);
}

/* returns index of where current node nodesPID[] */
int get_pid_nodeIndex()
{
    int i = 0;
    for (i = 0; i < par->SO_NODES_NUM; i++)
    {
        if (nodesPID[i].pid == myPID)
            return i;
    }
    return ERROR;
}

/* CTRL-C handler */
void node_interrupt_handle(int signum)
{
    int nodeIndex = get_pid_nodeIndex();
    TEST_ERROR
    write(1, "::NODE:: SIGINT received\n", 26);

    sem_reserve(semPIDs_ID, 1);
    TEST_ERROR
    nodesPID[nodeIndex].balance = currBalance;
    sem_release(semPIDs_ID, 1);
    TEST_ERROR

    msgctl(queueID, IPC_RMID, NULL);
    TRACE(("[NODE] queue removed\n"))
    TEST_ERROR
    exit(0);
}

int main(int argc, char *argv[])
{
    transaction **blockBuffer = malloc(sizeof(transaction) * (SO_BLOCK_SIZE - 1));
    block *newBlock = malloc(sizeof(block));

    struct timespec randSleepTime;
    struct timespec sleepTimeRemaining;
    struct sigaction saINT_node;

    myPID = getpid();
    currBalance = 0;

    bzero(&saINT_node, sizeof(saINT_node));

    attach_ipc_objects(argv);
    signal_handler_init(&saINT_node); /* no idea why it isn't working, it's literally the same implementation as user */
    TRACE(("[NODE %d] sighandler init\n", myPID));

    message_queue_init();
    TEST_ERROR
    while (1)
    {
        SLEEP_TIME_SET

        fill_block_buffer(blockBuffer);
        TEST_ERROR
        new_block(blockBuffer, newBlock);
        TEST_ERROR

        sem_reserve(semLedger_ID, 1);
        TEST_ERROR
        append_block_to_ledger(newBlock);
        TEST_ERROR

        SLEEP
        sem_release(semLedger_ID, 1);
        TEST_ERROR
    }
}
#include "include/common.h"
#include "include/nodes.h"

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
block *ledger;

int semPIDs_ID;
int semLedger_ID;
int queueID;

pid_t myPID;

pool transPool;

/* acts as buffer for currently fetched message to be copied into pool */
struct msgbuf_trans fetchedMex;

/*
 ======================
 ||  POOL FUNCTIONS  ||
 ======================
 */

/* initialize linked list transaction pool */
void transaction_pool_init()
{
    transPool.head = NULL;
    transPool.tail = NULL;
    transPool.size = 0;
}

/* append a msgbuf_trans to pool */
int add_to_pool()
{
    struct msgbuf_trans *newTransaction = malloc(sizeof(struct msgbuf_trans));
    if (newTransaction == NULL)
        TRACE(("*** [NODE %d] malloc failed, system out of memory ***", myPID))

    newTransaction->transactionMessage = fetchedMex.transactionMessage;
    newTransaction->transactionMessage.next = NULL;

    /* there's already some transactions in pool so I attach the pointer to this
     * new transaction to the next of the last
     */
    if (transPool.tail != NULL)
    {
        transPool.tail->transactionMessage.next = newTransaction;
    }

    transPool.tail = newTransaction;

    /* if head is NULL then this is the first transaction */
    if (transPool.head == NULL)
    {
        transPool.head = newTransaction;
    }
    return SUCCESS;
}

/* remove a msgbuf_trans from pool, returns directly the transaction associated */
transaction remove_from_pool()
{
    struct msgbuf_trans *tmp = transPool.head;
    transaction poppedTrans = tmp->transactionMessage.userTrans;

    if (transPool.head == NULL){
        poppedTrans.amount = ERROR;
        return poppedTrans;
    }
        

    transPool.head = transPool.head->transactionMessage.next;

    /* if head is NULL tail shoul become NULL too */
    if (transPool.head == NULL)
        transPool.tail = NULL;

    free(tmp);
    return poppedTrans;
}

/*
 ======================
 || QUEUE FUNCTIONS  ||
 ======================
 */

/* attaches to message queue initialized with myPID as key */
void message_queue_attach()
{
    queueID = msgget(myPID, 0);
    TEST_ERROR
}

/* process starts fetching transactions from it's msg_q until transPool is full */
void fetch_messages()
{
    if (transPool.size < par->SO_TP_SIZE)
    {
        TRACE(("[NODE %d] Trying to receive message of size(transaction) from queue %d\n", myPID, queueID))
        msgrcv(queueID, &fetchedMex, sizeof(struct msgbuf_trans), TRANSACTION_MTYPE, 0);
        TEST_ERROR
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
        add_to_pool();
        transPool.size++;
    }
}

/*
 ======================
 || LEDGER FUNCTIONS ||
 ======================
 */

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
}

/* fills the buffer with SO_BLOCK_SIZE-1 transactions */
void fill_block_transList(transaction *transListWithoutReward)
{
    int i;
    TRACE(("[NODE %d] is starting to process a block\n", myPID))

    for (i = 0; i < (SO_BLOCK_SIZE - 1); i++)
    {
        transListWithoutReward[i] = remove_from_pool();
    }
}

void insert_block_in_ledger(block *newBlock)
{
    int i;
    for (i = 0; i < SO_REGISTRY_SIZE; i++){
        /* a bit of and hack: shm segments are 0ed and our processes can't have pid 0 */
        if(ledger[i].transList[1].sender == 0){ 
            sem_reserve(semLedger_ID, 1);
            ledger[i] = *newBlock;
            sem_release(semLedger_ID, 1);
            return SUCCESS;
        }
    }

    printf("[NODE %d] tried to add block but ledger is full\n", myPID);
    killpg(0, SIGINT);
}

/*
 ======================
 ||  NODE FUNCTIONS  ||
 ======================
 */

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

/* attaches ipc objects based on IDs passed via arguments */
void attach_ipc_objects(char **argv)
{
    par = shmat(PARAMETERS_ARGV, NULL, 0);
    TEST_ERROR
    usersPID = shmat(USERS_PID_ARGV, NULL, 0);
    TEST_ERROR
    nodesPID = shmat(NODES_PID_ARGV, NULL, 0);
    TEST_ERROR
    ledger = shmat(LEDGER_ARGV, NULL, 0);
    TEST_ERROR
    semPIDs_ID = SEM_PIDS_ARGV;
    semLedger_ID = SEM_LEDGER_ARGV;
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

/* initializes signal handlers for SIGINT */
void signal_handler_init(struct sigaction *saINT_node)
{
    saINT_node->sa_handler = node_interrupt_handle;
    sigaction(SIGINT, saINT_node, NULL);
}

/* CTRL-C handler */
void node_interrupt_handle(int signum)
{
    int nodeIndex = get_pid_nodeIndex();
    TEST_ERROR
    write(1, "::NODE:: SIGINT received\n", 26);

    /*sem_reserve(semPIDs_ID, 1);*/
    TEST_ERROR
    nodesPID[nodeIndex].balance = currBalance;
    /*sem_release(semPIDs_ID, 1);*/
    TEST_ERROR

    msgctl(queueID, IPC_RMID, NULL);
    TRACE(("[NODE] queue removed\n"))
    TEST_ERROR

    report_mem_leak_nodes();
    exit(0);
}

void node_sigchld_handle(int signum);

int main(int argc, char *argv[])
{
    transaction **transBuffer = malloc(sizeof(transaction) * (SO_BLOCK_SIZE - 1));

    struct timespec randSleepTime;
    struct timespec sleepTimeRemaining;
    struct sigaction saINT_node;

    myPID = getpid();
    currBalance = 0;

    bzero(&saINT_node, sizeof(saINT_node));

    attach_ipc_objects(argv);
    signal_handler_init(&saINT_node); /* no idea why it isn't working, it's literally the same implementation as user */
    TRACE(("[NODE %d] sighandler init\n", myPID));

    message_queue_attach();
    transaction_pool_init();
    TEST_ERROR
    while (1)
    {

        /*
         * msgrcv transactions in loop until pool is full
         * if size is >= SO_BLOCK_SIZE-1 fork and create a block
         * append to ledger said block
         * SLEEP
         * exit(0)
         * if process is killed it will receive a SIGCHLD signal
         * it means that the process was killed while processing a block
         * need to manage
         */

        fetch_messages();
        if (transPool.size >= (SO_BLOCK_SIZE - 1))
        {
            switch (fork())
            {
            case -1: /* error */
                printf("[NODE %d] error while forking to create a block\n");
                break;

            case 0: /* child creates a new block and appends it to ledger */
                SLEEP_TIME_SET
                block *newBlock = malloc(sizeof(block));

                fill_block_transList(transBuffer);
                new_block(transBuffer, newBlock);
                insert_block_in_ledger(newBlock);

                free(newBlock);

                SLEEP
                exit(0);
                break;

            default: /* parent case */
                break;
            }
        }
    }
}
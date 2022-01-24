#include "include/common.h"
#include "include/nodes.h"
#include "include/print.h"
#include "utils/pool.h"

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
 || QUEUE FUNCTIONS  ||
 ======================
 */

/* attaches to message queue initialized with myPID as key */
void message_queue_attach()
{
    do
    {
        queueID = msgget(myPID, 0);
        TEST_ERROR
    } while (errno == ENOENT);
}

/* process starts fetching transactions from it's msg_q until transPool is full */
void fetch_messages()
{
    if (transPool.size < par->SO_TP_SIZE)
    {
        msgrcv(queueID, &fetchedMex, sizeof(struct msgbuf_trans), TRANSACTION_MTYPE, 0);
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
            break;
        }
        TRACE(("[NODE %d] received %d UC to process from [USER %d] to [USER %d]\n", myPID, fetchedMex.transactionMessage.userTrans.amount, fetchedMex.transactionMessage.userTrans.sender, fetchedMex.transactionMessage.userTrans.receiver))
        add_to_pool(&transPool, &fetchedMex);
        transPool.size++;
    }
    /*else
    {
        TRACE(("[NODE %d] pool is full\n", myPID))
    }*/
}

/*
 ======================
 || LEDGER FUNCTIONS ||
 ======================
 */

/* initializes new block with transList[0] as reward transaction */
void new_block(transaction *blockTransaction, block *newBlock)
{
    int i;
    transaction reward;
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);

    reward.timestamp = timestamp;
    reward.sender = SELF;
    reward.receiver = getpid();
    reward.amount = sum_reward(blockTransaction); /*sum of each reward of transaction in the block */
    reward.reward = 0;
    reward.status = pending;

    newBlock->transList[0] = reward;

    for (i = 1; i < SO_BLOCK_SIZE; i++)
    {
        newBlock->transList[i] = blockTransaction[i - 1];
    }

    /*print_block(newBlock);*/
}

/* fills the buffer with SO_BLOCK_SIZE-1 transactions */
void fill_block_transList(transaction *transListWithoutReward)
{
    int i;
    TRACE(("[NODE %d] is starting to process a block\n", myPID))

    for (i = 0; i < (SO_BLOCK_SIZE - 1); i++)
    {
        transListWithoutReward[i] = remove_from_pool(&transPool);
        transPool.size--;
    }
}

void confirm_block(block *toConfirm){
    int i;
    for (i = 0; i < SO_BLOCK_SIZE; i++){
        toConfirm->transList[i].status = confirmed;
    }
}

void insert_block_in_ledger(block *newBlock)
{
    int i;
    block tmp;
    for (i = 0; i < SO_REGISTRY_SIZE; i++)
    {
        /* a bit of and hack: shm segments are 0ed and our processes can't have pid 0 */
        if (ledger[i].transList[1].sender == 0)
        {
            tmp = *newBlock;
            tmp.blockIndex = i;

            sem_reserve(semLedger_ID, 1);
            ledger[i] = tmp;
            confirm_block(&ledger[i]);
            sem_release(semLedger_ID, 1);

            return;
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
int sum_reward(transaction *sumBlock)
{
    int i = 0;
    int sum = 0;

    for (i = 0; i < (SO_BLOCK_SIZE - 1); i++)
    {
        sum += sumBlock[i].reward;
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
    ledger = (block *)shmat(LEDGER_ARGV, NULL, 0);
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

    exit(0);
}

void node_sigchld_handle(int signum);

int main(int argc, char *argv[])
{
    transaction transBuffer[sizeof(transaction) * (SO_BLOCK_SIZE - 1)];

    struct timespec randSleepTime;
    struct timespec sleepTimeRemaining;
    struct sigaction saINT_node;

    myPID = getpid();
    currBalance = 0;

    bzero(&saINT_node, sizeof(saINT_node));

    attach_ipc_objects(argv);
    
    signal_handler_init(&saINT_node);
    message_queue_attach();
    transaction_pool_init(&transPool);
    TEST_ERROR
    while (1)
    {

        /*
         * if process is killed it will receive a SIGCHLD signal
         * it means that the process was killed while processing a block
         * need to manage
         */

        fetch_messages();
        if (transPool.size >= (SO_BLOCK_SIZE - 1))
        {
            /* this removes SO-BLOCK-SIZE-1 transactions from pool */
            fill_block_transList(transBuffer);
            switch (fork())
            {
            case -1: /* error */
                TRACE(("[NODE %d] error while forking to create a block\n", myPID));
                break;

            case 0: /* child creates a new block and appends it to ledger */
            {
                block *newBlock = malloc(sizeof(block));

                SLEEP_TIME_SET;

                new_block(transBuffer, newBlock);
                insert_block_in_ledger(newBlock);

                free(newBlock);

                SLEEP;
                exit(0);
                break;
            }

            default: /* parent case */
                break;
            }
        }
    }

    TRACE(("[NODE %d] somehow I broke free from an endless while loop\n", myPID))
}
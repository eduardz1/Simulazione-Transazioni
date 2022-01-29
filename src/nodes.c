#include "include/common.h"
#include "include/nodes.h"
#include "include/print.h"
#include "utils/pool.h"

/*
 ======================
 || GLOBAL VARIABLES ||
 ======================
 */

struct parameters *par;
user *usersPID;
node *nodesPID;
pid_t *friendList;
int friendList_size;
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
    } while (errno == ENOENT);
}

/* process starts fetching transactions from it's msg_q until transPool is full */
void fetch_messages() /* need also to fetch friends just in case */
{
    static int flag = 20; /* every 20 cycles it will send a transaction to a friend */
    static struct msgbuf_friends friendReceived;
    int sizeofFriend = friendList_size;

    TRACE(("[NODE %d] flag: %d\n", myPID, flag))
    if (transPool.size < par->SO_TP_SIZE && flag < 20)
    {
        /* This configuration causes deadlock when two friends are present on the
         * queue one after another, given that only the master can send new friends
         * to the nodes and the latter are extracted randomly, the deadlock is very
         * unlikely to occour due to the frequency of which the transactions are
         * sent on the queue compared to new friends from master.
         * Solving the deadlock is just a matter of setting IPC_NOWAIT to the receive
         * of transactions but the performance are so much worse that we end up
         * processing only about half of the blocks
         */
        if (receive_message(queueID, &fetchedMex, sizeof(struct msgbuf_trans), TRANSACTION_MTYPE, 0) == 0)
        {
            TRACE(("[NODE %d] received %d UC to process from [USER %d] to [USER %d]\n", myPID, fetchedMex.transactionMessage.userTrans.amount, fetchedMex.transactionMessage.userTrans.sender, fetchedMex.transactionMessage.userTrans.receiver))
            add_to_pool(&transPool, &fetchedMex);
            transPool.size++;
        }

        /* don't wait for new friends, just fetch them if they are in queue */
        if (receive_message(queueID, &friendReceived, sizeof(struct msgbuf_friends), FRIENDS_MTYPE, IPC_NOWAIT) == 0)
        {
            TRACE(("[NODE %d] I made a new friend!\n", myPID))
            friendList = realloc(friendList, sizeofFriend * sizeof(pid_t) + 1);
            friendList_size++;
            friendList[sizeofFriend] = friendReceived.friend;
        }
        flag++;
    }
    else
    { /* this basically prevents the node from ever being full, the most edge
       * case is that in which tail gets sent to master but it can't create
       * new overbuf nodes but in that case the transaction gets discarded
       */
        send_to_random_friend();
        flag = 0;
    }
}

void send_to_random_friend()
{
    int queue;
    int i;
    struct msgbuf_trans tMex;
    BZERO(&tMex, sizeof(tMex));
    if (remove_from_pool(&transPool, &tMex) == ERROR)
        return;

    transPool.size--;
    i = RAND(0, par->SO_FRIENDS_NUM - 1);
    tMex.mtype = TRANSACTION_MTYPE;

    if (tMex.transactionMessage.hops <= 0)
    {
        TRACE(("[NODE %d] asking master to create new node\n", myPID))
        tMex.transactionMessage.hops = 0;
        queue = getppid();
        TRACE(("[NODE %d] ppid() %d\n", myPID, queue))
        queue = msgget(queue, 0);
        if (send_message(queue, &tMex, sizeof(struct msgbuf_trans), IPC_NOWAIT) == SUCCESS)
        {
            TRACE(("[NODE %d] a transaction reached maximum hops and was sent to master\n", myPID))
        } /* else the transaction is discarded */
        return;
    }

    tMex.transactionMessage.hops--;

    if (friendList[i] == 0) /* an error occurred and a friend was not initialized correctly */
        kill(myPID, SIGINT);
    queue = msgget(friendList[i], 0);
    /* instead of checking in master we just ignore same PID */
    if (queue == queueID)
    {
        add_to_pool(&transPool, &tMex);
        transPool.size++;
        return;
    }

    if (send_message(queue, &tMex, sizeof(struct msgbuf_trans), IPC_NOWAIT) == 0)
    {
        TRACE(("[NODE %d] sent a transaction to friend %d\n", myPID, friendList[i]))
    }
    else
    {
        TRACE(("[NODE %d] failed to send a transaction to friend %d\n", myPID, friendList[i]))
        add_to_pool(&transPool, &tMex);
        transPool.size++;
    }
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
    BZERO(&timestamp, sizeof(timestamp));
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
    struct msgbuf_trans tmp;
    BZERO(&tmp, sizeof(tmp))

    TRACE(("[NODE %d] is starting to process a block\n", myPID))

    for (i = 0; i < (SO_BLOCK_SIZE - 1); i++)
    {
        if (remove_from_pool(&transPool, &tmp) == ERROR)
        {
            TRACE(("*** [NODE %d] fatal error ***\n", myPID))
            kill(myPID, SIGINT);
        }

        transListWithoutReward[i] = tmp.transactionMessage.userTrans;
        transPool.size--;
    }
}

void fill_friendList(pid_t *friendList)
{
    unsigned int i;
    struct msgbuf_friends friendMex;
    BZERO(&friendMex, sizeof(friendMex))

    TRACE(("[NODE %d] trying to receive friends\n", myPID))
    /* waits for a FRIENDS_MTYPE transactions before continuing */
    for (i = 0; i < par->SO_FRIENDS_NUM; i++)
    {
        receive_message(queueID, &friendMex, sizeof(struct msgbuf_friends), FRIENDS_MTYPE, 0);
        friendList[i] = friendMex.friend;

        TRACE(("[NODE %d] friend list is: %d\n", myPID, friendList[i]))
    }
}

void confirm_block(block *toConfirm)
{
    int i;
    for (i = 0; i < SO_BLOCK_SIZE; i++)
    {
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

    sem_reserve(semPIDs_ID, 1);
    nodesPID[get_pid_nodeIndex()].balance += sum;
    sem_release(semPIDs_ID, 1);

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
    unsigned int i = 0;
    for (i = 0; i < par->SO_NODES_NUM * 2; i++)
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
#ifdef DEBUG
    /* cast return value into the void, ! is needed because of gcc behaviour */
    (void)!write(2, "::NODE:: SIGINT received\n", 25);
#endif

    TRACE(("[NODE %d] key of my queue %d\n", myPID, queueID))
    msgctl(queueID, IPC_RMID, NULL);
    TRACE(("[NODE] queue removed\n"))

    exit(0);
}

int main(int argc, char *argv[])
{
    transaction transBuffer[sizeof(transaction) * (SO_BLOCK_SIZE - 1)];

    struct timespec randSleepTime;
    struct timespec sleepTimeRemaining;
    struct sigaction saINT_node;
    BZERO(&randSleepTime, sizeof(randSleepTime));
    BZERO(&sleepTimeRemaining, sizeof(sleepTimeRemaining));
    BZERO(&saINT_node, sizeof(saINT_node));

    myPID = getpid();

    if (argc == 0)
    {
        printf("[NODE %d] no arguments passed, can't continue like this any more :C\n", myPID);
        return ERROR;
    }

    attach_ipc_objects(argv);
    srand(getpid());
    signal_handler_init(&saINT_node);
    message_queue_attach();

    friendList = malloc(par->SO_FRIENDS_NUM * sizeof(pid_t));
    friendList_size = par->SO_FRIENDS_NUM;
    fill_friendList(friendList);

    transaction_pool_init(&transPool);
    while (1)
    {

        /*
         * if process is killed it will receive a SIGCHLD signal
         * it means that the process was killed while processing a block
         * need to manage
         */

        fetch_messages();

        sem_reserve(semPIDs_ID, 1);
        nodesPID[get_pid_nodeIndex()].tpSize = transPool.size;
        sem_release(semPIDs_ID, 1);

        if (transPool.size >= (SO_BLOCK_SIZE - 1))
        {
            /* this removes SO-BLOCK-SIZE-1 transactions from pool */
            fill_block_transList(transBuffer);
            switch (fork())
            {
            case -1: /* error */
                TRACE(("[NODE %d] error while forking to create a block\n", myPID))
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
}
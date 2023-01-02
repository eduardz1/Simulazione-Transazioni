#include "include/nodes.h"
#include "../utils/debug.h"
#include "../utils/msg.h"
#include "../utils/sem.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/shm.h>

#define FREQUENCY                                                              \
    20 /* how many times fetch_messages() is called before a transaction is    \
          sent to a random friend */

struct Params *par;
User *users;
Node *nodes;
Ledger *ledger;

int semNodesPIDs_ID;
int semLedger_ID;
int queueID;

pid_t myPID;

ArrayList *friends;   // ArrayList of pid_t
ArrayList *transPool; // ArrayList of struct MsgBufTransData

/* attaches to message queue initialized with myPID as key */
void message_queue_attach(void)
{
    do
    {
        queueID = msgget(myPID, 0);
    } while (errno == ENOENT);
}

/* process starts fetching transactions from it's msg_q until transPool is full
 */
void fetch_messages(void) /* need also to fetch friends just in case */
{
    static int flag = FREQUENCY;

    /* this basically prevents the node from ever being full, the most edge
     * case is that in which tail gets sent to master, but it can't create
     * new overbuf nodes but in that case the transaction gets discarded
     */
    if (transPool->length >= par->SO_TP_SIZE || flag >= 20)
    {
        send_to_random_friend();
        flag = 0;
        return;
    }

    pid_t *friendReceived;
    struct MsgbufTransData *fetchedMex;

    struct MsgbufTrans buft;
    if (receive_message(queueID,
                        &buft,
                        sizeof(struct MsgbufTransData),
                        TRANSACTIONS_MTYPE,
                        0) == EXIT_SUCCESS)
    {
        safe_alloc(fetchedMex, 0, sizeof(struct MsgbufTransData))
        memcpy(fetchedMex, &buft.data, sizeof(struct MsgbufTransData));
        TRACE(("[NODE %d] received %d UC to process from [USER %d] to "
               "[USER %d]\n",
               myPID,
               fetchedMex->transaction.amount,
               fetchedMex->transaction.sender,
               fetchedMex->transaction.receiver))
        arraylist_append(transPool, fetchedMex);
    }

    struct MsgbufFriends buff;
    if (receive_message(
            queueID, &buff, sizeof(pid_t), FRIENDS_MTYPE, IPC_NOWAIT) == 0)
    {
        safe_alloc(friendReceived, 0, sizeof(pid_t))
        *friendReceived = buff.friend_;
        TRACE(("[NODE %d] I made a new friend_!\n", myPID))
        arraylist_append(friends, &friendReceived);
    }
    flag++;
}

void send_to_random_friend(void)
{
    int queue;
    int i;
    struct MsgbufTrans tMex;
    if (transPool->length == 0) return;

    memcpy(&tMex.data, transPool->data[0], sizeof(struct MsgbufTransData));
    arraylist_remove(transPool, 0);

    i = RAND(0, par->SO_FRIENDS_NUM - 1);
    tMex.mtype = TRANSACTIONS_MTYPE;

    if (tMex.data.hops <= 0)
    {
        TRACE(("[NODE %d] asking master to create new node\n", myPID))
        tMex.data.hops = 0;
        queue = getppid();
        queue = msgget(queue, 0);
        if (send_message(
                queue, &tMex, sizeof(struct MsgbufTransData), IPC_NOWAIT) ==
            EXIT_SUCCESS)
        {
            TRACE(("[NODE %d] a transaction reached maximum hops and was sent "
                   "to master\n",
                   myPID))
        } /* else the transaction is discarded */
        return;
    }

    tMex.data.hops--;

    if (!*((pid_t *)friends->data[i])) /* an error occurred and a friend_
                               was not initialized correctly */
        kill(myPID, SIGINT);
    queue = msgget(*((pid_t *)friends->data[i]), 0);
    /* instead of checking in master we just ignore same PID */
    if (queue == queueID)
    {
        arraylist_append(transPool, &tMex);
        return;
    }

    if (!send_message(queue, &tMex, sizeof(struct MsgbufTransData), IPC_NOWAIT))
    {
        TRACE(("[NODE %d] sent a transaction to friend_ %d\n",
               myPID,
               *((pid_t *)friends->data[i])))
    }
    else
    {
        TRACE(("[NODE %d] failed to send a transaction to friend_ %d\n",
               myPID,
               *((pid_t *)friends->data[i])))
        arraylist_append(transPool, &tMex);
    }
}

/* initializes new block with transList[0] as reward transaction */
void new_block(Transaction *blockTransaction, Block *newBlock)
{
    int i;
    Transaction reward;
    struct timespec timestamp = {0};
    clock_gettime(CLOCK_REALTIME, &timestamp);

    reward.timestamp = timestamp;
    reward.sender = SELF;
    reward.receiver = myPID;
    reward.amount = sum_reward(blockTransaction); /*sum of each reward of
                                                     transaction in the block */
    reward.reward = 0;
    reward.status = PENDING;

    newBlock->transList[0] = reward;

    for (i = 1; i < SO_BLOCK_SIZE; i++)
    {
        newBlock->transList[i] = blockTransaction[i - 1];
    }
}

/* fills the buffer with SO_BLOCK_SIZE-1 transactions */
void fill_block_transList(Transaction *transListWithoutReward)
{
    TRACE(("[NODE %d] is starting to process a block\n", myPID))

    for (size_t i = 0; i < (SO_BLOCK_SIZE - 1); i++)
    {
        if (transPool->length == 0) break;

        struct MsgbufTransData *buf = transPool->data[0];
        Transaction tmp = buf->transaction;

        TRACE(("[NODE %d] is processing transaction %d:\n"
               "\ttmp.sender   = %d\n"
               "\ttmp.receiver =  %d\n"
               "\ttmp.amount   = %d\n",
               myPID,
               i,
               tmp.sender,
               tmp.receiver,
               tmp.amount))

        memcpy(transListWithoutReward + i, &tmp, sizeof(Transaction));
        arraylist_remove(transPool, 0);
    }

    TRACE(("[NODE %d] finished processing a block:\n"
           "\ttransListWithoutReward[0].sender   = %d,\n"
           "\ttransListWithoutReward[0].receiver = %d,\n"
           "\ttransListWithoutReward[0].amount   = %d\n",
           myPID,
           transListWithoutReward[0].sender,
           transListWithoutReward[0].receiver,
           transListWithoutReward[0].amount))
}

void fill_friendList(ArrayList *friendList)
{
    pid_t *friendMex;

    TRACE(("[NODE %d] trying to receive friends\n", myPID))
    /* waits for a FRIENDS_MTYPE transactions before continuing */
    for (size_t i = 0; i < par->SO_FRIENDS_NUM; i++)
    {
        safe_alloc(friendMex, 0, sizeof(pid_t))
        receive_message(queueID, friendMex, sizeof(pid_t), FRIENDS_MTYPE, 0);
        arraylist_append(friendList, friendMex);
        TRACE(("[NODE %d] friendlist[%d]: %d\n",
               myPID,
               i,
               *((pid_t *)friendList->data[i])))
    }
}

void confirm_block(Block *toConfirm)
{
    for (size_t i = 0; i < SO_BLOCK_SIZE;
         toConfirm->transList[i++].status = CONFIRMED)
        ;
}

void insert_block_in_ledger(Block *newBlock)
{
    for (size_t i = 0; i < SO_REGISTRY_SIZE; i++)
    {
        /* a bit of and hack: shm segments are 0ed and our processes can't have
         * pid 0 */
        if (ledger->blockList[i].transList[1].sender == 0)
        {
            sem_reserve(semLedger_ID, 1);
            memcpy(&ledger->blockList[i], newBlock, sizeof(Block));
            confirm_block(&ledger->blockList[i]);
            sem_release(semLedger_ID, 1);

            return;
        }
    }

    TRACE(("[NODE %d] tried to add block but ledger is full\n", myPID));
    killpg(0, SIGINT);
}

/* sums rewards of sumBlock[SO_BLOCK_SIZE-1] transactions */
int sum_reward(Transaction *sumBlock)
{
    int sum = 0;

    for (size_t i = 0; i < (SO_BLOCK_SIZE - 1); i++)
    {
        sum += sumBlock[i].reward;
    }

    sem_reserve(semNodesPIDs_ID, 1);
    nodes[get_pid_nodeIndex()].balance += sum;
    sem_release(semNodesPIDs_ID, 1);

    return sum;
}

/* attaches ipc objects based on IDs passed via arguments */
void attach_ipc_objects(char **argv)
{
    par = shmat(PARAMETERS_ARGV, NULL, 0);
    TEST_ERROR
    users = shmat(USERS_PID_ARGV, NULL, 0);
    TEST_ERROR
    nodes = shmat(NODES_PID_ARGV, NULL, 0);
    TEST_ERROR
    ledger = (Ledger *)shmat(LEDGER_ARGV, NULL, 0);
    TEST_ERROR
    semNodesPIDs_ID = SEM_NODES_PIDS_ARGV;
    semLedger_ID = SEM_LEDGER_ARGV;
}

/* returns index of where current node nodesPID[] */
int get_pid_nodeIndex()
{
    for (size_t i = 0; i < par->SO_NODES_NUM * 2; i++)
    {
        if (nodes[i].pid == myPID) return i;
    }
    return EXIT_FAILURE;
}

/* initializes signal handlers for SIGINT */
void signal_handler_init(struct sigaction *sa)
{
    sa->sa_handler = node_interrupt_handle;
    sigaction(SIGINT, sa, NULL);
}

/* CTRL-C handler */
void node_interrupt_handle(int signum)
{
    int i, accurateBalance = 0;
    UNUSED(signum);

#ifdef DEBUG
    /* cast return value into the void, ! is needed because of gcc behaviour */
    (void)!write(2, "::NODE:: SIGINT received\n", 25);
#endif

    msgctl(queueID, IPC_RMID, NULL);
    TRACE(("[NODE] queue removed\n"))

    /* traverse ledger because only the balance written on ledger at the end is
     * the one that should be displayed
     */
    for (i = 0; i < SO_REGISTRY_SIZE; i++)
    {
        if (ledger->blockList[i].transList[0].receiver == myPID)
            accurateBalance += ledger->blockList[i].transList[0].amount;
    }
    sem_reserve(semNodesPIDs_ID, 1);
    nodes[get_pid_nodeIndex()].balance = accurateBalance;
    sem_release(semNodesPIDs_ID, 1);

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    if (argc != IPC_NUM + 1) // TODO: fa un po'schifo
    {
        fprintf(stderr,
                "usage: <shmUsers_ID> <shmNodes_ID> <shmParams_ID> "
                "<shmLedger_ID> <semUsersPIDs_ID> <semLedger_ID> "
                "<semNodesPIDs_ID>\n");
        exit(EXIT_FAILURE);
    }

    Transaction transBuffer[sizeof(Transaction) * (SO_BLOCK_SIZE - 1)];
    struct sigaction sa = {0};

    myPID = getpid();

    attach_ipc_objects(argv);
    srand(getpid());
    signal_handler_init(&sa);
    message_queue_attach();

    transPool = arraylist_new(0);
    friends = arraylist_new(par->SO_FRIENDS_NUM * sizeof(pid_t));
    fill_friendList(friends);

    while (true)
    {
        fetch_messages();

        sem_reserve(semNodesPIDs_ID, 1);
        nodes[get_pid_nodeIndex()].tpSize = transPool->length;
        sem_release(semNodesPIDs_ID, 1);

        if (transPool->length >= (SO_BLOCK_SIZE - 1)) // to account for reward
                                                      // transaction
        {
            TRACE(("[NODE %d] can finally process a block\n", myPID))
            fill_block_transList(transBuffer);

            Block newBlock;
            new_block(transBuffer, &newBlock);
            insert_block_in_ledger(&newBlock);

            SLEEP;
        }
    }
}

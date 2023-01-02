#include "include/users.h"
#include "../utils/arraylist.h"
#include "../utils/debug.h"
#include "../utils/msg.h"
#include "../utils/sem.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/shm.h>

#define REWARD(amount, reward) (ceil(((reward * (amount)) / 100.0)))

/* parameters of simulation */
struct Params *par;
User *users;
Node *nodes;
Ledger *ledger;

int semUsersPIDs_ID;
int semLedger_ID;
int queueID;

ArrayList *outGoingTransactions; /* ArrayList of Transaction */

uint32_t currBalance;
pid_t myPID;
struct MsgbufTrans transMsg;

/* returns index of where user with PID_toSearch is located in usersPID[] */
int get_pid_userIndex(int PID_toSearch)
{
    uint32_t i;

    for (i = 0; i < par->SO_USER_NUM; i++)
    {
        if (users[i].pid == PID_toSearch) return i;
    }

    return EXIT_FAILURE;
}

/* returns a random PID of a non-dead user from usersPID[] */
pid_t get_random_userPID()
{
    int index;
    pid_t val = 0;

    do
    {
        index = RAND(0, par->SO_USER_NUM - 1);
        if (users[index].status != dead && users[index].pid != myPID)
            val = users[index].pid;
    } while (!val);

    return val;
}

/* returns a random PID of an available node from nodesPID[] */
pid_t get_random_nodePID()
{
    int index;
    pid_t val = 0;

    do
    {
        index = RAND(0, par->SO_NODES_NUM * 2 - 1);
        if (nodes[index].status == available && nodes[index].pid != 0)
            val = nodes[index].pid;
    } while (!val);

    return val;
}

int compare_transactions(void *t1, void *t2)
{
    if (t1 == NULL || t2 == NULL) return 0;

    Transaction *trans1 = (Transaction *)t1;
    Transaction *trans2 = (Transaction *)t2;

    return (trans1->timestamp.tv_nsec == trans2->timestamp.tv_nsec &&
            trans1->timestamp.tv_sec == trans2->timestamp.tv_sec &&
            trans1->amount == trans2->amount &&
            trans1->receiver == trans2->receiver &&
            trans1->sender == trans2->sender &&
            trans1->reward == trans2->reward);
}

/* safely updates status of user to statusToSet: 0 alive, 1 broke, 2 dead */
void update_status(int statusToSet)
{
    int i = get_pid_userIndex(myPID);
    if (i == -1)
    {
        TRACE(("[USER %d] failed to find myself in usersPID[]", myPID))
    }

    sem_reserve(semUsersPIDs_ID, 1);
    users[i].status = statusToSet;
    if (statusToSet == 2)
    {
        TRACE(("[USERS] dead increased\n"))
    }
    sem_release(semUsersPIDs_ID, 1);
}

/* attaches ipc objects based on IDs passed via arguments */
void attach_ipc_objects(char **argv)
{
    par = (struct Params *)shmat(PARAMETERS_ARGV, NULL, 0);
    TEST_ERROR
    users = (User *)shmat(USERS_PID_ARGV, NULL, 0);
    TEST_ERROR
    nodes = (Node *)shmat(NODES_PID_ARGV, NULL, 0);
    TEST_ERROR
    ledger = (Ledger *)shmat(LEDGER_ARGV, NULL, 0);
    TEST_ERROR
    semUsersPIDs_ID = SEM_USERS_PIDS_ARGV;
    semLedger_ID = SEM_LEDGER_ARGV;
}

/* try to attach to queue of nodePID key until it succeeds */
void queue_to_pid(pid_t nodePID)
{
    do
    {
        errno = 0;
        queueID = msgget(nodePID, 0);
    } while (errno == ENOENT);
}

/* initializes transaction values and sets it to pending */
void transaction_init(pid_t userPID, int amount, int reward)
{
    struct timespec exactTime = {0};
    clock_gettime(CLOCK_REALTIME, &exactTime);

    transMsg.mtype = TRANSACTIONS_MTYPE;
    transMsg.data.transaction.sender = myPID;
    transMsg.data.transaction.receiver = userPID;
    transMsg.data.transaction.amount = amount;
    transMsg.data.transaction.reward = reward;
    transMsg.data.transaction.status = PENDING;

    transMsg.data.transaction.timestamp = exactTime;
    transMsg.data.hops = par->SO_HOPS;
}

/* initializes signal handlers for SIGINT and SIGUSR1 */
void signal_handlers_init(struct sigaction *saUSR1, struct sigaction *saINT)
{
    /* -- SIGNAL HANDLERS --
     * first set all bytes of sigation to 0
     * then initialize sa.handler to a pointer to
     * the function user_transaction/interrupt_handle
     * then set the handler to handle SIUSR1/SIGINT signals
     * ((struct sigaction *oldact) = NULL)
     */
    saUSR1->sa_handler = user_transactions_handle;
    saINT->sa_handler = user_interrupt_handle;
    sigaction(SIGUSR1, saUSR1, NULL);
    sigaction(SIGINT, saINT, NULL);
}

/* send transaction currTrans to user userPID via node nodePID */
int send_transaction(uint32_t *retry)
{
    pid_t userPID = get_random_userPID();
    pid_t nodePID = get_random_nodePID();

    uint32_t amount = RAND(2, currBalance);
    uint32_t reward = REWARD(amount, par->SO_REWARD);
    amount -= reward;

    queue_to_pid(nodePID);
    transaction_init(userPID, amount, reward);

    Transaction *sent;

    SLEEP // simulates time to send transaction

        if (send_message(queueID,
                         &transMsg,
                         sizeof(struct MsgbufTransData),
                         IPC_NOWAIT) == 0)
    {

        TRACE(("[USER %d] sent a transaction of %d UC to [USER %d] via queue "
               "%d\n",
               myPID,
               transMsg.data.transaction.amount,
               transMsg.data.transaction.receiver,
               queueID))
        currBalance -= (transMsg.data.transaction.amount +
                        transMsg.data.transaction.reward);

        safe_alloc(sent, 0, sizeof(Transaction));
        memcpy(sent, &transMsg.data.transaction, sizeof(Transaction));

        arraylist_append(outGoingTransactions, sent);
        *retry = par->SO_RETRY;

        return EXIT_SUCCESS;
    }

    if (!--retry) // terminates when retry reaches 0
    {
        update_status(dead);
        TRACE(("[USER %d] max retry reached, terminating\n", myPID))
        kill(myPID, SIGINT);
    }

    return EXIT_FAILURE;
}

/* safely updates balance of user */
void update_balance(uint32_t tempBalance)
{
    int i = get_pid_userIndex(myPID);

    sem_reserve(semUsersPIDs_ID, 1);
    currBalance = tempBalance;
    users[i].balance = currBalance;
    sem_release(semUsersPIDs_ID, 1);

    TRACE(("[USER %d] balance updated to %d\n", myPID, currBalance))
}

/* saves balance of calling user in currBalance */
void get_balance()
{
    size_t i, j;
    long accumulate = 0; /* needs to fit two uint32_ts inside */

    /* balance is buffered in tempBalance so that if the program is interrupted
     * while get_balance() is running the user doesn't suddenly get his
     * balance reset to SO_BUDGET_INIT
     */
    uint32_t tempBalance = par->SO_BUDGET_INIT;

    /* create a local copy to avoid inconsistencies, we could use semaphores
     * but parsing the entire ledger and the outGoing list takes an awful lot
     * and would make our program very inefficient, memory is relatively cheap,
     * even with real blockchains you have to save the entire chain to the hard
     * drive if you want to make a local keychain so it should make sense
     */
    Ledger tmp;
    sem_reserve(semLedger_ID, 1);
    memcpy(&tmp, ledger, sizeof(Ledger));
    sem_release(semLedger_ID, 1);
    TRACE(("[USER %d] ledger copied\n", myPID))
    for (i = 0; i < SO_REGISTRY_SIZE; i++)
    {
        /* can't have pid = 0 unless the block is not initialized */
        if (tmp.blockList[i].transList[0].sender == 0) break;

        for (j = 1; j < SO_BLOCK_SIZE; j++)
        {
            if (tmp.blockList[i].transList[j].sender == myPID)
            {
                int index = arraylist_index_of(outGoingTransactions,
                                               &compare_transactions,
                                               &tmp.blockList[i].transList[j]);
                if (index != -1) arraylist_remove(outGoingTransactions, index);

                accumulate -= (tmp.blockList[i].transList[j].amount +
                               tmp.blockList[i].transList[j].reward);
            }
            else if (tmp.blockList[i].transList[j].receiver == myPID)
            {
                accumulate += tmp.blockList[i].transList[j].amount;
            }
        }
    }

    for (i = 0; i < outGoingTransactions->length; i++)
    {
        accumulate -= (((Transaction *)outGoingTransactions->data[i])->amount +
                       ((Transaction *)outGoingTransactions->data[i])->reward);
    }

    if (accumulate * (-1) > tempBalance)
    {
        TRACE(("*** [USER %d] error in calculating balance: %d, overflow ***\n",
               myPID,
               accumulate * (-1))); // FIXME: sometimes results in overflow
        update_status(2);
        killpg(0, SIGINT);
    }

    tempBalance += accumulate;
    if (errno == ERANGE) /* not working as intended */
    {
        TRACE(("[USER %d] went out of bound, punishment for being that rich "
               "is death\n",
               myPID));
        update_status(2);
        kill(myPID, SIGINT);
    }

    update_balance(tempBalance);
}

/* SIGUSR1 handler, sends a transaction */
void user_transactions_handle(int signum)
{
    static uint32_t retry = 1;
    UNUSED(signum);
    write(1, "::USER:: SIGUSR1 received\n", 27);

    get_balance();
    if (currBalance >= 2)
    {
        update_status(alive);
        send_transaction(&retry);
    }
    else
    {
        write(1, "::USER:: sorry balance too low\n", 32);
    }
}

/* CTRL-C handler */
void user_interrupt_handle(int signum)
{
    int i = get_pid_userIndex(myPID);
    size_t j;
    UNUSED(signum);

#ifdef DEBUG
    (void)!write(2, "::USER:: SIGINT received\n", 25);
#endif

    get_balance();

    if (users[get_pid_userIndex(myPID)].status != dead)
    {
        if (users[i].balance >= 2)
            update_status(alive);
        else
            update_status(broke);
    }

    sem_reserve(semUsersPIDs_ID, 1);
    for (j = 0; j < outGoingTransactions->length; j++)
    {
        users[i].balance +=
            (((Transaction *)outGoingTransactions->data[j])->amount +
             ((Transaction *)outGoingTransactions->data[j])->reward);
    }

    sem_release(semUsersPIDs_ID, 1);

    arraylist_free(outGoingTransactions);
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

    uint32_t retry;

    struct sigaction saUSR1 = {0};
    struct sigaction saINT_user = {0};

    myPID = getpid(); /* set myPID value */

    outGoingTransactions = arraylist_new(0);

    srand(myPID); /* seed random number generator */

    attach_ipc_objects(argv);
    signal_handlers_init(&saUSR1, &saINT_user);
    transMsg.mtype = TRANSACTIONS_MTYPE;

    currBalance = par->SO_BUDGET_INIT;
    retry = par->SO_RETRY;

    while (true)
    {
        get_balance();
        if (currBalance >= 2)
        {
            update_status(alive);
            send_transaction(&retry);
        }
        else
        {
            update_status(broke);

            if (!--retry) // leave if you want being broke to count as a try
            {
                update_status(dead);
                TRACE(("[USER %d] max retry reached, terminating\n", myPID))
                kill(myPID, SIGINT);
            }

            SLEEP
        }
    }
}

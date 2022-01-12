#include "include/common.h"

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

    newLedger->head = new_block();
    newLedger->registryCurrSize = 1;

    newLedger = (ledger *)shmat(shmID, NULL, 0);

    return newLedger;
}

block *new_block()
{
    block *newBlock = malloc(sizeof(block));
    transaction reward;
    struct timespec timestamp;

    /* memset(newBlock->transList, 0, SO_BLOCK_SIZE); */
    clock_gettime(CLOCK_REALTIME, &timestamp);

    reward.timestamp = timestamp;
    reward.sender = SELF;
    reward.receiver = getpid();
    reward.amount = 0;
    reward.reward = 0;

    newBlock->transList[0] = reward;
    newBlock->blockIndex = 0;
    newBlock->next = NULL;

    return newBlock;
}

void add_transaction_to_block(block *block, transaction *newTrans, int index)
{
    block->transList[index] = *newTrans; /* ye probably we don't need a whole ass function for that*/
}

void send_transaction(pid_t sender, pid_t receiver, int quantity, int reward)
{
    struct timespec tp;
    transaction currentTransaction;
    /*
     * get current time and save it in a struct timespec
     * inside the transaction struct
     */

    clock_gettime(CLOCK_REALTIME, &tp);

    currentTransaction.timestamp = tp;
}
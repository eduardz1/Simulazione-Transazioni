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

    newLedger->head = NULL;
    newLedger->registryCurrSize = 1;

    newLedger = (ledger *)shmat(shmID, NULL, 0);

    return newLedger;
}
int sum_reward(transaction** sumBlock)
{
    int i=0;
    int sumReward;
    for(i=0;i<SO_BLOCK_SIZE-1;i++)
    {
        sumReward+=sumBlock[i]->reward;
    }
    return sumReward;
}
block *new_block(transaction** blockTransaction)
{

   block *newBlock= malloc(sizeof(block));
    transaction reward;
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);

    reward.timestamp= timestamp;
    reward.sender= SELF;
    reward.receiver=getpid();
    reward.amount= sum_reward(blockTransaction); /*sum of each reward of transaction in the block */
    reward.reward=0;

    memcpy(newBlock->transList+1, *blockTransaction,(SO_BLOCK_SIZE-1)*(sizeof(transaction)));
    newBlock->next = NULL;

    return newBlock;
}

void add_transaction_to_block(block *block, transaction *newTrans, int index)
{
    block->transList[index] = *newTrans; /* ye probably we don't need a whole ass function for that*/
}
#include "include/common.h"

ledger *ledger_init()
{
    ledger *ledger;
    int shmID; /* ID of "ledger" shared memory segment */

    /* -- LEDGER INITIALIZATION --
     * save the ID of our new (IPC_PRIVATE) shared memory segment of size -ledger-
     * smctl will deallocate the shared memory segment only when every process detaches it
     * tells OS that ledger of type ledger is our shared memory of shmID
     */
    shmID = shmget(IPC_PRIVATE, sizeof(ledger), 0600); /* 0600 dummy value */
    shmctl(shmID, IPC_RMID, NULL);

    ledger->head = new_block();
    ledger->shmID = shmID;
    ledger->registryCurrSize = 1;

    shmat(shmID, NULL, 0);

    return ledger;
}

struct block *new_block()
{
    struct block *newBlock;
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

void add_transaction_to_block(struct block *block, transaction *newTrans, int index)
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

/*void printLists(){
    struct node *first=first;
        while(first!=NULL){
            printf("%d,%d",first->timeStamp, first->sender, first->receiver);
    }
	
}*/

/*void formatted_timestamp()
{
    clock_t tic = clock();
    clock_t start = clock();
    clock_t stop = clock();

    time_t rawtime;
    time_t now;
    struct tm *info;
    struct tm *today;
    double elapsed;
    char buf[128];

    time(&now);
    today = localtime(&now);
    strftime(buf, 128, "%Y/%m/%d", today);
    printf("%s\n", buf);

    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC; /* time ./a.out*
}*/
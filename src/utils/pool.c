#include "pool.h"

/* initialize linked list transaction pool */
void transaction_pool_init(pool *transPool)
{
    transPool->head = NULL;
    transPool->tail = NULL;
    transPool->size = 0;
}

/* append a msgbuf_trans to pool */
int add_to_pool(pool *transPool, struct msgbuf_trans *message)
{
    struct msgbuf_trans *newTransaction = malloc(sizeof(struct msgbuf_trans));
    if (newTransaction == NULL)
    {
        TRACE(("*** malloc failed in %s:%d, system out of memory ***\n", __FILE__, __LINE__))
    }

    newTransaction->transactionMessage = message->transactionMessage;
    newTransaction->transactionMessage.next = NULL;

    /* there's already some transactions in pool so I attach the pointer to this
     * new transaction to the next of the last
     */
    if (transPool->tail != NULL)
    {
        transPool->tail->transactionMessage.next = newTransaction;
    }

    transPool->tail = newTransaction;

    /* if head is NULL then this is the first transaction */
    if (transPool->head == NULL)
    {
        transPool->head = newTransaction;
    }
    return SUCCESS;
}

/* remove a msgbuf_trans from pool, returns directly the transaction associated */
int remove_from_pool(pool *transPool, struct msgbuf_trans *tMex)
{
    if (transPool == NULL)
    {
        return ERROR;
    }
    else if (transPool->head == NULL)
    {
        return ERROR;
    }

    *tMex = *transPool->head;
    free(transPool->head);
    transPool->head = transPool->head->transactionMessage.next;

    /* if head is NULL tail shoul become NULL too */
    if (transPool->head == NULL)
        transPool->tail = NULL;

    return SUCCESS;
}
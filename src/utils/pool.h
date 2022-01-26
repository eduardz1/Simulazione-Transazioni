#ifndef SIMULAZIONE_TRANSAZIONI_POOL_H
#define SIMULAZIONE_TRANSAZIONI_POOL_H

#include "../include/common.h"

typedef struct pool_t
{
    struct msgbuf_trans *head;
    struct msgbuf_trans *tail;
    unsigned int size;
} pool;

void transaction_pool_init(pool *transPool);
int add_to_pool(pool *transPool, struct msgbuf_trans *message);
struct msgbuf_trans remove_from_pool(pool *transPool);

#endif /* SIMULAZIONE_TRANSAZIONI_POOL_H */
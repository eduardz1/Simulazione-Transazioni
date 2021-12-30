/* Common header that multiple processes need to use */
#ifndef SIMULAZIONE_TRANSAZIONI_COMMON_H
#define SIMULAZIONE_TRANSAZIONI_COMMON_H

#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#ifndef NULL
#define NULL 0 /* thre's a problem with NULL for some reason */
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define SO_BLOCK_SIZE 100 /* number of transaction per block*/
#define SO_REGISTRY_SIZE 1000 /* max length of consecutive blocks */
#define SELF -1

/* Transaction struct */
typedef struct transaction_t
{
    struct timespec timestamp;
    pid_t sender;
    pid_t receiver;
    int amount;
    int reward;
    enum {
        pending,
        processing,
        confirmed,
        aborted,
    } status;   
} transaction;

/* Block struct */
typedef struct block_t
{
    transaction transList[SO_BLOCK_SIZE];
    unsigned int blockIndex; /* when a block is written on ledger it's Index needs to be updated */
    block *next;
} block;

/* Libro Mastro (ledger) struct */
typedef struct ledger_t
{
    block *head;
    int shmID; /* ID of the shared memory segment */
    unsigned int registryCurrSize; /* initialize to SO_REGISTRY_SIZE, update with every new block added */
} ledger;


ledger *ledger_init();
block *new_block();
void add_block(block);
void add_transaction_to_block(block*, transaction*, int index);
void add_block_to_ledger(block*);
void find_transaction(struct timespec timestamp, pid_t sender, pid_t receiver); /* NULL used to group results */
void send_transaction(pid_t sender, pid_t receiver, int quantity, int reward);

#endif /* SIMULAZIONE_TRANSAZIONI_COMMON_H */
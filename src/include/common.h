/* Common header that multiple processes need to use */
#ifndef SIMULAZIONE_TRANSAZIONI_COMMON_H
#define SIMULAZIONE_TRANSAZIONI_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#ifndef NULL
#define NULL 0 /* thre's a problem with NULL for some reason */
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define RAND(min, max) ((rand() % (max - min + 1)) + min)

#define SO_BLOCK_SIZE 100     /* number of transaction per block*/
#define SO_REGISTRY_SIZE 1000 /* max length of consecutive blocks */
#define SELF -1

#define TEST_ERROR                                 \
    if (errno)                                     \
    {                                              \
        fprintf(stderr,                            \
                "%s:%d: PID=%5d: Error %d (%s)\n", \
                __FILE__,                          \
                __LINE__,                          \
                getpid(),                          \
                errno,                             \
                strerror(errno));                  \
    }

struct parameters
{
    int SO_USER_NUM;
    int SO_NODES_NUM;
    int SO_BUDGET_INIT;
    int SO_REWARD;
    int SO_MIN_TRANS_GEN_NSEC;
    int SO_MAX_TRANS_GEN_NSEC;
    int SO_RETRY;
    int SO_TP_SIZE;
    int SO_MIN_TRANS_PROC_NSEC;
    int SO_MAX_TRANS_PROC_NSEC;
    int SO_SIM_SEC;
    int SO_FRIENDS_NUM;
    int SO_HOPS;
} par;

/* Transaction struct */
typedef struct transaction_t
{
    struct timespec timestamp;
    pid_t sender;
    pid_t receiver;
    int amount;
    int reward;
    enum
    {
        pending,
        processing,
        confirmed,
        aborted
    } status;
} transaction;

/* Block struct */
struct block
{
    transaction transList[SO_BLOCK_SIZE];
    unsigned int blockIndex; /* when a block is written on ledger it's Index needs to be updated */
    struct block *next;
};

/* Libro Mastro (ledger) struct */
typedef struct ledger_t
{
    struct block *head;
    int shmID;                     /* ID of the shared memory segment */
    unsigned int registryCurrSize; /* initialize to SO_REGISTRY_SIZE, update with every new block added */
} ledger;

ledger *ledger_init();
struct block *new_block();
void add_block(struct block);
void add_transaction_to_block(struct block *, transaction *, int index);
void add_block_to_ledger(struct block *);
void find_transaction(struct timespec timestamp, pid_t sender, pid_t receiver); /* NULL used to group results */
void send_transaction(pid_t sender, pid_t receiver, int quantity, int reward);

#endif /* SIMULAZIONE_TRANSAZIONI_COMMON_H */
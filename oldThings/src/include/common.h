#ifndef SIMULAZIONE_TRANSAZIONI_COMMON_H
#define SIMULAZIONE_TRANSAZIONI_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>

#include "../utils/debug.h"
#include "../utils/sem.h"

#ifndef NULL
#define NULL 0 /* thre's a problem with NULL for some reason */
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define "RAND(min, max) ((rand() % (max - min + 1)) + min)

/* -- IPC OBJECTS -- */
#define IPC_ERROR -1
#define SHM_PARAMETERS 1337
#define SHM_USERS_ARRAY 1338
#define SHM_NODES_ARRAY 1339
#define SHM_LEDGER 1340
#define SEM_USERS_PIDS_KEY 420
#define SEM_NODES_PIDS_KEY 421
#define SEM_LEDGER_KEY 422
#define M_QUEUE_KEY 0x5AD

#define SO_BLOCK_SIZE 10  /* number of transaction per block*/
#define SO_REGISTRY_SIZE 1000 /* max length of consecutive blocks */
#define SELF -1
#define EVERYONE_BROKE '$'
#define TRANSACTION_MTYPE 28410
#define FRIENDS_MTYPE 28411

/* -- ARGV LOCATION OF IPC OBJECTS -- */
#define USERS_PID_ARGV (atoi(argv[1]))
#define NODES_PID_ARGV (atoi(argv[2]))
#define PARAMETERS_ARGV (atoi(argv[3]))
#define LEDGER_ARGV (atoi(argv[4]))
#define SEM_USERS_PIDS_ARGV (atoi(argv[5]))
#define SEM_LEDGER_ARGV (atoi(argv[6]))
#define SEM_NODES_PIDS_ARGV (atoi(argv[7]))

/* -- USER RETURN STATUS -- */
#define WENT_BROKE 1
#define MAX_RETRY 2
#define ERROR -1
#define SUCCESS 0

extern int errno;

#ifndef TEST_ERROR
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
#endif

/* bzero() is not a standard C function, contrary to memset().
 * there's some bugs that can be avoided doing it that way 
 */
#define BZERO(x, x_size) memset(x, 0, x_size);

struct parameters
{
    unsigned int SO_USER_NUM;
    unsigned int SO_NODES_NUM;
    unsigned int SO_BUDGET_INIT;
    char SO_REWARD; /* max value 100 */
    unsigned long SO_MIN_TRANS_GEN_NSEC;
    unsigned long SO_MAX_TRANS_GEN_NSEC;
    unsigned int SO_RETRY;
    unsigned int SO_TP_SIZE;
    unsigned long SO_MIN_TRANS_PROC_NSEC;
    unsigned long SO_MAX_TRANS_PROC_NSEC;
    unsigned int SO_SIM_SEC;
    unsigned int SO_FRIENDS_NUM;
    unsigned int SO_HOPS;
};

typedef struct user_t
{
    pid_t pid;
    unsigned int balance;
    enum
    {
        alive,
        broke,
        dead
    } status;
} user;

typedef struct node_t
{
    pid_t pid;
    unsigned long balance;
    int tpSize;
    enum
    {
        available,
        full
    } status;
} node;

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
typedef struct block_t
{
    transaction transList[SO_BLOCK_SIZE];
    unsigned int blockIndex; /* when a block is written on ledger it's Index needs to be updated */
} block;

struct msgbuf_trans
{
    long mtype;
    struct message
    {
        int hops; /* number of times the transaction has hopped */
        transaction userTrans;
        struct msgbuf_trans *next; /*pointer to the next message in the pool*/
    } transactionMessage;
};

struct msgbuf_friends
{
    long mtype;
    pid_t friend;
};

enum term
{
    timeElapsed,
    usersDead,
    ledgerFull
};

#endif /* SIMULAZIONE_TRANSAZIONI_COMMON_H */

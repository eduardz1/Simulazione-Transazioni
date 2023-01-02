#ifndef SIMULAZIONE_TRANSAZIONI_COMMON_H
#define SIMULAZIONE_TRANSAZIONI_COMMON_H

#include <errno.h>
#include <stdint.h>
#include <sys/types.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define RAND(min, max) ((rand() % ((max) - (min) + 1)) + (min))

#define SO_BLOCK_SIZE 10      /* number of transaction per block*/
#define SO_REGISTRY_SIZE 1000 /* max length of consecutive blocks */
#define SELF (-1)
#define FRIENDS_MTYPE 0x5AD
#define TRANSACTIONS_MTYPE 0xBAD

#define SHM_NUM 4
#define SEM_NUM 3
#define IPC_NUM (SHM_NUM + SEM_NUM)

/* -- ARGV LOCATION OF IPC OBJECTS -- */
#define USERS_PID_ARGV (atoi(argv[1]))
#define NODES_PID_ARGV (atoi(argv[2]))
#define PARAMETERS_ARGV (atoi(argv[3]))
#define LEDGER_ARGV (atoi(argv[4]))
#define SEM_USERS_PIDS_ARGV (atoi(argv[5]))
#define SEM_LEDGER_ARGV (atoi(argv[6]))
#define SEM_NODES_PIDS_ARGV (atoi(argv[7]))

/* bzero() is not a standard C function, contrary to memset().
 * there's some bugs that can be avoided doing it that way
 */
#define BZERO(x, x_size) memset(x, 0, x_size);

#define UNUSED(x) (void)(x)

/* https://stackoverflow.com/questions/35237503/sizeof-anonymous-nested-struct
 */
#define sizeof_field(s, m) (sizeof((((s *)0)->m)))

/* specify nmemb if you want to use calloc instead of malloc, leave 0 otherwise
 */
#define safe_alloc(x, nmemb, size)                                             \
    do                                                                         \
    {                                                                          \
        x = nmemb ? calloc(nmemb, size) : malloc(size);                        \
        if (x == NULL)                                                         \
        {                                                                      \
            perror("alloc");                                                   \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    } while (0);

struct Params
{
    uint32_t SO_USER_NUM;
    uint32_t SO_NODES_NUM;
    uint32_t SO_BUDGET_INIT;
    uint8_t SO_REWARD; /* max value 100 */
    uint64_t SO_MIN_TRANS_GEN_NSEC;
    uint64_t SO_MAX_TRANS_GEN_NSEC;
    uint32_t SO_RETRY;
    uint32_t SO_TP_SIZE;
    uint64_t SO_MIN_TRANS_PROC_NSEC;
    uint64_t SO_MAX_TRANS_PROC_NSEC;
    uint32_t SO_SIM_SEC;
    uint32_t SO_FRIENDS_NUM;
    uint32_t SO_HOPS;
};

typedef struct User_t
{
    pid_t pid;
    uint32_t balance;
    enum
    {
        alive,
        broke,
        dead
    } status;
} User;

typedef struct Node_t
{
    pid_t pid;
    uint64_t balance;
    int tpSize;
    enum
    {
        available,
        full
    } status;
} Node;

typedef struct Transaction_t
{
    struct timespec timestamp;
    pid_t sender;
    pid_t receiver;
    int amount;
    int reward;
    enum
    {
        PENDING,
        PROCESSING,
        CONFIRMED,
        ABORTED
    } status;
} Transaction;

typedef struct Block_t
{
    Transaction transList[SO_BLOCK_SIZE];
} Block;

typedef struct Ledger_t
{
    Block blockList[SO_REGISTRY_SIZE];
} Ledger;

struct MsgbufTransData
{
    uint32_t hops; /* initialized to max hops */
    Transaction transaction;
};
struct MsgbufTrans
{
    long mtype;
    struct MsgbufTransData data;
};

struct MsgbufFriends
{
    long mtype;
    pid_t friend_;
};

#endif /* SIMULAZIONE_TRANSAZIONI_COMMON_H */

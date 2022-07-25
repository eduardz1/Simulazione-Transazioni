/* #define _XOPEN_SOURCE 700 */
/*#define _GNU_SOURCE*/ 
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
/* per funzioni time/timespect*/
#include <time.h>
#include <aio.h> 

/* #include <sys/stat.h> 
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
/*CONFIGURATION PARAMETERS THROW GLOBAL VARIABLE */ 
#define     SO_USERS_NUM            atoi(getenv("SO_USERS_NUM"))
#define     SO_NODES_NUM            atoi(getenv("SO_NODES_NUM"))
#define     SO_REWARD               atoi(getenv("SO_REWARD"))
#define     SO_RETRY                atoi(getenv("SO_RETRY"))
#define     SO_TP_SIZE              atoi(getenv("SO_TP_SIZE"))
#define     SO_BUDGET_INIT          atoi(getenv("SO_BUDGET_INIT"))
#define     SO_SIM_SEC              atoi(getenv("SO_SIM_SEC"))
#define     SO_MIN_TRANS_GEN_NSEC   atoi(getenv("SO_MIN_TRANS_GEN_NSEC"))
#define     SO_MAX_TRANS_GEN_NSEC   atoi(getenv("SO_MAX_TRANS_GEN_NSEC"))
#define     SO_MIN_TRANS_PROC_NSEC  atoi(getenv("SO_MIN_TRANS_PROC_NSEC"))
#define     SO_MAX_TRANS_PROC_NSEC  atoi(getenv("SO_MAXC_TRANS_PROC_NSEC"))

#define     SO_BLOCK_SIZE           10
#define     SO_REGISTRY_SIZE        100
#define     MSG_KEY                 12345
#define     SM_KEY                  ((key_t)5077)
#define     SEM_KEY                 ((key_t)5078)


/* test error preso da esecizi di esempio moodle */
#define TEST_ERROR    if (errno) {fprintf(stderr, \
					   "%s:%d: PID=%5d: Error %d (%s)\n",\
					   __FILE__,\
					   __LINE__,\
					   getpid(),\
					   errno,\
					   strerror(errno));}

/* struttura transazione */
 struct tr{
        struct timespec timestamp;
        int sender;
        int receiver;
        int quantita;
        int reward;
    };

/* struttura blocco*/
struct bk{
    struct tr elem[SO_BLOCK_SIZE];/*elem può contenere SO BLOCK SIZE transazioni*/
    struct timespec timestamp;
    int id;
};

/* struttura libro mastro */
struct m_b
{
   struct bk block[SO_REGISTRY_SIZE];
   int next; /* prossimo blocco da aggiungere */
   
};

typedef struct m_b * book;

struct msg_buf {
	long mtype;             /* message type, must be > 0 */
	struct tr transazione;    /* message data */
};

void one_for_all(int signum);

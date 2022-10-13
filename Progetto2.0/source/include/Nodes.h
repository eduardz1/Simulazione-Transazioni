#include "Conf.h"
#include "Master.h"
#include "../Util/IPCS_SEM.h"
#include "../Util/Ms_Queue.h"

/*user return status, used in tp */
#define WENT_BROKE 1
#define MAX_RETRY 2
#define ERROR -1
#define SUCCESS 0

#define SLEEP_TIME_SET        \
    randSleepTime.tv_sec = 0; \
    randSleepTime.tv_nsec = RAND(SO_MIN_TRANS_PROC_NSEC, SO_MAX_TRANS_PROC_NSEC);

#define SLEEP                                                                            \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &randSleepTime, &sleepTimeRemaining); \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTimeRemaining, NULL);

typedef struct tp_pool
{
    Message *head;
    Message *tail;
    unsigned int size;
} pool;

void transaction_pool_init(pool *transPool);
int add_to_pool(pool *transPool, Message *message);
int remove_from_pool(pool *transPool, Message *message);
int sum_reward(transaction *);
int get_pid_node_index();
void message_queue_attach();
void Message_Rec();
void take_transaction();
void Block(transaction *blockT, Block_ *newBlock);
void transListTo_block(transaction *Noreward);
void fill_friends(pid_t *friendList);
void confirm_state_block(Block_ *confirmed);
void block_ledger(Block_ *newBlock);
void ipc_attach_argv(char **argv);
void sig_handler_init(struct sigaction *sa);
void node_handler_interrupt(int sigum);

int send_message(int queueID,void *msg,int size,int flag);
int receive_message(int queueID,void *msg,int size,int mtype,int flag);
/*

struct msqid_dis{
    struct ipc_perm mesg_perm;
    time_t msg_stime;
    time_t msg_rtime;
    time_t msg_ctime;
    msgqnum_t msg_qnum;
    msglen_t msg_qbytes;
    pid_t msg_lspid;
    pid_t msg_lrpid;
};
*/
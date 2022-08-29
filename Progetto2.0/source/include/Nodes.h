
#include "../Util/transaction.h"

/*user return status, used in tp */
#define WENT_BROKE 1
#define MAX_RETRY 2
#define ERROR -1
#define SUCCESS 0


#define SLEEP                                                                            \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &randSleepTime, &sleepTimeRemaining); \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTimeRemaining, NULL);
#define SLEEP_TIME_SET        \
    randSleepTime.tv_sec = 0; \
    randSleepTime.tv_nsec = RAND(SO_MIN_TRANS_PROC_NSEC, SO_MAX_TRANS_PROC_NSEC);

typedef struct tp_pool
{
    struct msgbuf_trans *head;
    struct msgbuf_trans *tail;
    unsigned int size;
}pool;

void transaction_pool_init(pool *transPool);
int add_to_pool(pool *transPool,struct msgbuf_trans *message);
int remove_from_pool(pool *transPool, struct msgbuf_trans *message);
int sum_reward(transaction* sumBlock); 
void Message_Queue();
void message_queue_attach(); 
void Message_Rec(); 
void take_transaction(); 
void Block(transaction * blockT, Block_ *newBlock); 
void transListTo_block(transaction * Noreward);
void fill_friends(pid_t * friendList) ;
void confirm_state_block(Block_ * confirmed); 
void block_ladger(Block_ *newBlock);

void shm_attach(char **argv) ; 
void get_pid_node_index(); 
void sig_handler_init(struct sigaction * saint_node) ; 
void node_handler_interrupt(int sigum);





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

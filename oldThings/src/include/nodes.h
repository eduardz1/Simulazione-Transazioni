/*#ifndef SIMULAZIONE_TRANSAZIONI_NODES_H
#define SIMULAZIONE_TRANSAZIONI_NODES_H*/ 
#include "../utils/msg.h"

/* sets sleep time with nsec precision for trans_proc */
#define SLEEP_TIME_SET        \
    randSleepTime.tv_sec = 0; \
    randSleepTime.tv_nsec = RAND(par->SO_MIN_TRANS_PROC_NSEC, par->SO_MAX_TRANS_PROC_NSEC);

#define SLEEP                                                                            \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &randSleepTime, &sleepTimeRemaining); \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTimeRemaining, NULL);

void message_queue_attach();
void fetch_messages();
void send_to_random_friend();

void signal_handler_init(struct sigaction *saINT);
void node_interrupt_handle(int signum);
void attach_ipc_objects(char **argv);
void new_block(transaction* blockTransaction, block *newBlock);
void confirm_block(block *toConfirm);
void insert_block_in_ledger(block *newBlock);
void fill_block_transList(transaction *buffer);
void fill_friendList(pid_t *friendList);

int sum_reward(transaction *);
int get_pid_nodeIndex();



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

/* #endif /* SIMULAZIONE_TRANSAZIONI_NODES_H */ */ 
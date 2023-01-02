#ifndef SIMULAZIONE_TRANSAZIONI_NODES_H
#define SIMULAZIONE_TRANSAZIONI_NODES_H

#include "../../include/common.h"
#include "../../utils/arraylist.h"
#include <limits.h>
#include <signal.h>
#include <time.h>

/* sets sleep time with nsec precision for trans_proc */
#define SLEEP_TIME_SET(t)                                                      \
    do                                                                         \
    {                                                                          \
        _t.tv_sec = 0;                                                         \
        _t.tv_nsec =                                                           \
            RAND(par->SO_MIN_TRANS_PROC_NSEC, par->SO_MAX_TRANS_PROC_NSEC);    \
    } while (0);

#define SLEEP                                                                  \
    do                                                                         \
    {                                                                          \
        struct timespec _t = {0};                                              \
        struct timespec _remaining = {0};                                      \
        SLEEP_TIME_SET(_t)                                                     \
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &_t, &_remaining);      \
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &_remaining, NULL);     \
    } while (0);

void message_queue_attach(void);
void fetch_messages(void);
void send_to_random_friend(void);

void signal_handler_init(struct sigaction *saINT);
void node_interrupt_handle(int signum);

void new_block(Transaction *blockTransaction, Block *newBlock);
void confirm_block(Block *toConfirm);
void insert_block_in_ledger(Block *newBlock);

void fill_block_transList(Transaction *buffer);
void fill_friendList(ArrayList *friendList);
int sum_reward(Transaction *);

void attach_ipc_objects(char **argv);
int get_pid_nodeIndex(void);

#endif /* SIMULAZIONE_TRANSAZIONI_NODES_H */

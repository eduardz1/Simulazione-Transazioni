#ifndef SIMULAZIONE_TRANSAZIONI_USERS_H
#define SIMULAZIONE_TRANSAZIONI_USERS_H

#include "../../include/common.h"
#include <signal.h>
#include <time.h>
// FIXME: following macros can go in common.h

/* sets sleep time with nsec precision for trans_gen */
#define SLEEP_TIME_SET(t)                                                      \
    do                                                                         \
    {                                                                          \
        _t.tv_sec = 0;                                                         \
        _t.tv_nsec =                                                           \
            RAND(par->SO_MIN_TRANS_GEN_NSEC, par->SO_MAX_TRANS_GEN_NSEC);      \
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

void signal_handlers_init(struct sigaction *saUSR1, struct sigaction *saINT);
void user_transactions_handle(int signum);
void user_interrupt_handle(int signum);

int get_pid_userIndex(int PID_toSearch);
pid_t get_random_userPID(void);
pid_t get_random_nodePID(void);
void queue_to_pid(pid_t nodePID);

void transaction_init(pid_t nodePID, int amount, int reward);
int send_transaction(uint32_t *retry);
int search_trans_list(Block *blockToSearch);

void update_status(int statusToSet);
void attach_ipc_objects(char **argv);

void get_balance(void);
void update_balance(uint32_t tempBalance);

#endif /* SIMULAZIONE_TRANSAZIONI_USERS_H */

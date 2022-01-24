#ifndef SIMULAZIONE_TRANSAZIONI_USERS_H
#define SIMULAZIONE_TRANSAZIONI_USERS_H

#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <errno.h>

/* sets sleep time with nsec precision for trans_gen */
#define SLEEP_TIME_SET        \
    randSleepTime.tv_sec = 0; \
    randSleepTime.tv_nsec = RAND(par->SO_MIN_TRANS_GEN_NSEC, par->SO_MAX_TRANS_GEN_NSEC);

#define SLEEP                                                                            \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &randSleepTime, &sleepTimeRemaining); \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTimeRemaining, NULL);


/* Prototype*/

void user_transactions_handle(int signum);
void user_interrupt_handle(int signum);
int get_pid_userIndex(int PID_toSearch);
pid_t get_random_userPID();
pid_t get_random_nodePID();
void queue_to_pid(pid_t nodePID);
void transaction_init(pid_t nodePID, int amount, int reward);
int send_transaction();
void update_status(int statusToSet); 
void attach_ipc_objects(char **argv); 
void transaction_init(pid_t userPID, int amount, int reward); 
void signal_handlers_init(struct sigaction *saUSR1, struct sigaction *saINT);
int  search_trans_list(block *blockToSearch); 
void get_balance(); 
void update_balance(unsigned int tempBalance); 

#endif /* SIMULAZIONE_TRANSAZIONI_USERS_H */

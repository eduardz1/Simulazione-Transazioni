#ifndef SIMULAZIONE_TRANSAZIONI_USERS_H
#define SIMULAZIONE_TRANSAZIONI_USERS_H

/* sets sleep time with nsec precision for trans_gen */
#define SLEEP_TIME_SET        \
    randSleepTime.tv_sec = 0; \
    randSleepTime.tv_nsec = RAND(par->SO_MIN_TRANS_GEN_NSEC, par->SO_MAX_TRANS_GEN_NSEC);

#define SLEEP                                                                            \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &randSleepTime, &sleepTimeRemaining); \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTimeRemaining, NULL);

void user_transactions_handle(int signum);
void user_interrupt_handle(int signum);

int get_pid_userIndex(int PID_toSearch);
pid_t get_random_userPID();
pid_t get_random_nodePID();

void queue_to_pid(pid_t nodePID);
void transaction_init(pid_t nodePID, int amount, int reward);
int send_transaction();

#endif /* SIMULAZIONE_TRANSAZIONI_USERS_H */

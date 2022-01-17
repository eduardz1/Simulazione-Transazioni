#ifndef SIMULAZIONE_TRANSAZIONI_NODES_H
#define SIMULAZIONE_TRANSAZIONI_NODES_H

/* sets sleep time with nsec precision for trans_proc */
#define SLEEP_TIME_SET        \
    randSleepTime.tv_sec = 0; \
    randSleepTime.tv_nsec = RAND(par->SO_MIN_TRANS_PROC_NSEC, par->SO_MAX_TRANS_PROC_NSEC);

#define SLEEP                                                                            \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &randSleepTime, &sleepTimeRemaining); \
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTimeRemaining, NULL);

/*void signal_handler_init(struct sigaction *saINT); */
void node_interrupt_handle(int signum);

void new_block(transaction** blockTransaction, block *newBlock);
void fill_block_transList(transaction *buffer);

#endif /* SIMULAZIONE_TRANSAZIONI_NODES_H */
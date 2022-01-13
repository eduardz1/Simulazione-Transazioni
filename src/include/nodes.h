#ifndef SIMULAZIONE_TRANSAZIONI_NODES_H
#define SIMULAZIONE_TRANSAZIONI_NODES_H


struct timespec randSleepTime;
struct timespec sleepTimeRemaining;

/*void signal_handler_init(struct sigaction *saINT); */
void node_interrupt_handle(int signum);

#endif /* SIMULAZIONE_TRANSAZIONI_NODES_H */
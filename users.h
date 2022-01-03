#ifndef SIMULAZIONE_TRANSAZIONI_USERS_H
#define SIMULAZIONE_TRANSAZIONI_USERS_H

#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

void user_transactions_handle(int signum);

struct timespec randSleepTime;
struct timespec sleepTimeRemaining;

#endif /* SIMULAZIONE_TRANSAZIONI_USERS_H */
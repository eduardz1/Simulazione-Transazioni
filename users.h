#ifndef SIMULAZIONE_TRANSAZIONI_USERS_H
#define SIMULAZIONE_TRANSAZIONI_USERS_H

#include "balance.h"
#include "reward.h"
#include "arrayPID.h"
#include <signal.h>
#include <stdlib.h>

#define SO_MIN_TRANS_GEN_NSEC
#define SO_MAX_TRANS_GEN_NSEC
/*
 * NON active wait, the time is equivalent to the
 * verification algorithms that happen in "real" blockchains
 */
#define SO_BUDGET_INIT
#define SO_REWARD
#define SO_RETRY

struct rndTime {
    time_t tv_sec  = 0;
    long   tv_nsec = (rand() % (SO_MAX_TRANS_GEN_NSEC - SO_MIN_TRANS_GEN_NSEC + 1)) + SO_MIN_TRANS_GEN_NSEC;
    /* generate a random number form SO_MIN and SO_MAX using the rand() function that generates a random number
     * from 0 to RAND_MAX
     */
};

struct sigaction sa;
sa.sa_handler = user_transactions_handle;
sigaction(SIGUSR1, &sa, NULL);

void send_transaction();

void user_transactions_handle(int SIGUSR1);

#endif /* SIMULAZIONE_TRANSAZIONI_USERS_H */
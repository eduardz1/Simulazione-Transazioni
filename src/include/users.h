#ifndef SIMULAZIONE_TRANSAZIONI_USERS_H
#define SIMULAZIONE_TRANSAZIONI_USERS_H

#include "balance.h"
#include "reward.h"
#include "arrayPID.h"
#include <signal.h>
#include <stdlib.h>
#include <time.h>


struct sigaction sa;
sa.sa_handler = user_transactions_handle;
sigaction(SIGUSR1, &sa, NULL);

void send_transaction();

void user_transactions_handle(int SIGUSR1);

#endif /* SIMULAZIONE_TRANSAZIONI_USERS_H */

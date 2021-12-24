#ifndef SIMULAZIONE_TRANSAZIONI_USERS_H
#define SIMULAZIONE_TRANSAZIONI_USERS_H

#include "balance.h"
#include "reward.h"
#include "arrayPID.h"

#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

// void send_transaction();

void user_transactions_handle(int signum);

#endif /* SIMULAZIONE_TRANSAZIONI_USERS_H */

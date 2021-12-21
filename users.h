#include "balance.h"
#include "reward.h"
#include "arrayPID.h"
#include <signal.h>
#include <stdlib.h>

#define SO_MIN_TRANS_GEN_NSEC
#define SO_MAX_TRANS_GEN_NSEC

struct rndTime {
	time_t tv_sec  = 0;
	long   tv_nsec = (rand() % (SO_MAX_TRANS_GEN_NSEC - SO_MIN_TRANS_GEN_NSEC + 1)) + SO_MIN_TRANS_GEN_NSEC;
	/* generate a random number form SO_MIN and SO_MAX using the rand() function that generates a random number
	 * from 0 to RAND_MAX
	 */
}

struct sigaction sa;
sa.sa_handler = user_transactions_handle;
sigaction(SIGUSR1, &sa, NULL);

void send_transaction();

void user_transactions_handle(int SIGUSR1);

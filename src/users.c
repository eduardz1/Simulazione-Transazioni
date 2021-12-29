#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "include/users.h"
#include "include/transaction.h"
#include "include/reward.h"

#define RAND(min, max) (rand() % (max - min + 1)) + min
/*
 * NON active wait, the time is equivalent to the
 * verification algorithms that happen in "real" blockchains
 */

/*
 * Need to implement a way to send s transaction
 * signal, we can utilize a user defined signal
 * handler.
 * We also need to account for the signal SIGINT (CTRL-C).
 * Maybe we can implement some sort of graphic way to visualize
 * child processes (nodes and user) so that we can choose
 * the PID on which to send the signal to.
 * -- user-defined signal handlers are inherited by the child processes --
 * so it's better to handle them in the master program
 */

void user_transactions_handle(int signum)
{
	return send_transaction(getpid(), );
}

int main(int argc, char *argv[]) /* 
								  * (unsigned int SO_BUDGET_INIT, 
								  *  unsigned int SO_REWARD, 
								  *  unsigned int SO_RETRY, 
								  *  unsigned long SO_MIN_TRANS_GEN_NSEC, 
								  *  unsigned long SO_MAX_TRANS_GEN_NSEC) 
								  */
{
	srand(time(NULL)); /* initialize rand function */

	unsigned int budgetInit, reward, retry; /* command line int  */
	unsigned long minTransGen, maxTransGen; /* command line long */

	unsigned int currentBalance, amount;
	pid_t userPID, nodePID;

	struct timespec randSleepTime;
	struct timespec sleepTimeRemaining;
	struct sigaction sa;

	/* -- CL PARAMETERS INITIALIZATION -- */
	budgetInit = atoi(argv[1]);
	reward = atoi(argv[2]);
	retry = atoi(argv[3]);
	minTransGen = atol(argv[4]);
	maxTransGen = atol(argv[5]);

	/* -- SLEEP TIME SET --
	 * generate a random number form SO_MIN and SO_MAX using the rand() function that generates a random number
	 * from 0 to RAND_MAX
	 */
	randSleepTime.tv_sec = 0;
	randSleepTime.tv_nsec = RAND(minTransGen, maxTransGen);
	/*
	 * save the time unslept when interrupted by SIGUSR1 so that we can't force transactions at a much greater speed
	 * better to save it into a separate struct because clock_nanosleep will not update it if the sleep is not interrupted
	 */
	bzero(&sleepTimeRemaining, sizeof(sleepTimeRemaining));

	/* -- SIGNAL HANDLER --
	 * first set all bytes of sigation to 0
	 * then initialize sa.handler to a pointer to the function user_transaction_handle
	 * then set the handler to handle SIUSR1 signals ((struct sigaction *oldact) = NULL) 
	 */
	bzero(&sa, sizeof(sa));
	sa.sa_handler = user_transactions_handle;
	sigaction(SIGUSR1, &sa, NULL);

	currentBalance = balance(budgetInit);
	if (currentBalance < 2)
		return;
	
	/* get random pid from user group */
	userPID = randPID_u();
	/* get random pid from nodes group */	
	nodePID = randPID_n();
	/* get random int from 2 to currentBalance */
	amount = RAND(2, currentBalance);
	
	/* 
	 * rick needs to implement reward function
	 * reward = reward(amount, reward); 
	 */
	amount -= reward;

	while (retry--)
	{
		// transaction(nodePID, userPID);
		/* transaction(node_pid, user_pid); */
		/* wait(rand(SO_MIN_TRANS_GEN_NSEC, SO_MAX_GEN_NSEC) */
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &randSleepTime, &sleepTimeRemaining);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTimeRemaining, NULL);
	}
}
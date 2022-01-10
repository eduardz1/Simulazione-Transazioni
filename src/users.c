#include "include/common.h"
#include "include/users.h"

/*#define SO_USER_NUM (atoi(argv[1]))
#define SO_NODES_NUM (atoi(argv[2]))
#define SO_NUM_FRIENDS (atoi(argv[3]))
#define SO_SIM_SEC (atoi(argv[4]))

/* -- USER CL PARAMETERS -- *
#define SO_BUDGET_INIT (atoi(argv[5]))
#define SO_REWARD (atoi(argv[6]))
#define SO_RETRY (atoi(argv[7]))
#define SO_MIN_TRANS_GEN_NSEC (atol(argv[8]))
#define SO_MAX_TRANS_GEN_NSEC (atol(argv[9]))
*/
#define USERS_PID_ARRAY (atoi(argv[1]))
#define NODES_PID_ARRAY (atoi(argv[2]))
#define PARAMETERS (atoi(argv[3]))

#define REWARD(amount, reward) (ceil(((reward * (amount)) / 100.0)))
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
	/* return send_transaction(myPID, userPID, amount, reward); */
}

int main(int argc, char *argv[])
{
	unsigned int retry; /* command line int  */

	unsigned int currentBalance, amount, reward;
	pid_t myPID, userPID, nodePID;

	struct timespec randSleepTime;
	struct timespec sleepTimeRemaining;
	struct sigaction sa;
	struct parameters *par = shmat(PARAMETERS, NULL, 0);

	pid_t *usersPID = malloc(10 * sizeof(pid_t));
	pid_t *nodesPID = malloc(5 * sizeof(pid_t)); 

	usersPID = shmat(USERS_PID_ARRAY, NULL, 0);
	nodesPID = shmat(NODES_PID_ARRAY, NULL, 0);

	myPID = getpid();
	printf("User %d has finished", myPID);
    return;
	
	/* -- SIGNAL HANDLER --
	 * first set all bytes of sigation to 0
	 * then initialize sa.handler to a pointer to the function user_transaction_handle
	 * then set the handler to handle SIUSR1 signals ((struct sigaction *oldact) = NULL)
	 */
	bzero(&sa, sizeof(sa));
	sa.sa_handler = user_transactions_handle;
	sigaction(SIGUSR1, &sa, NULL);
	srand(time(NULL)); /* initialize rand function */

	retry = par->SO_RETRY;

	while (1)
	{

		/* -- SLEEP TIME SET --
		 * value for sec set to 0
		 * value for nsec is a random number from SO_MIN and SO_MAX
		 */
		randSleepTime.tv_sec = 0;
		randSleepTime.tv_nsec = RAND(par->SO_MIN_TRANS_GEN_NSEC, par->SO_MAX_TRANS_GEN_NSEC);
		/*
		 * save the time unslept when interrupted by SIGUSR1
		 * so that we can't force transactions at a much greater speed
		 * better to save it into a separate struct because clock_nanosleep
		 * will not update it if the sleep is not interrupted
		 */
		bzero(&sleepTimeRemaining, sizeof(sleepTimeRemaining));

		currentBalance = 100/*balance(myPID)*/;
		if (currentBalance < 2)
			return;

		/* get random pid from user group */
		userPID = usersPID[RAND(0, (par->SO_USER_NUM-1))];
		/* get random pid from nodes group */
		nodePID = nodesPID[RAND(0, (par->SO_NODES_NUM-1))];
		/* get random int from 2 to currentBalance */
		amount = RAND(2, currentBalance);

		reward = REWARD(amount, par->SO_REWARD);
		amount -= reward;

		/* need a pipe or something else to enable user to communicate with node
		 * and send it a send_transaction request
		 */
		send_transaction(myPID, userPID, amount, reward);

		/* if (aborted) /* pseudo code, node needs to send an aborted or accepted signal*
			retry--;
		else
			retry = SO_RETRY; */

		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &randSleepTime, &sleepTimeRemaining);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTimeRemaining, NULL);

		if (!retry)
			return;
	}
}
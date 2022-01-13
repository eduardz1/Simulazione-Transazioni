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

/* void wait_for_incoming_transaction() */

pid_t get_random_userPID(user *usersPID)
{
	int index;
	pid_t val = 0;

	do
	{
		index = RAND(0, ARRAY_SIZE(&usersPID) - 1);
		if (usersPID[index].status != dead)
			val = usersPID[index].pid;
	} while (!val);

	return val;
}

pid_t get_random_nodePID(node *nodesPID)
{
	int index;
	pid_t val = 0;

	do
	{
		index = RAND(0, ARRAY_SIZE(&nodesPID) - 1);
		if (nodesPID[index].status == available)
			val = nodesPID[index].pid;
	} while (!val);

	return val;
}

void user_transactions_handle(int signum)
{
	/* return send_transaction(myPID, userPID, amount, reward); */
}

void user_interrupt_handle(int signum)
{
	/* */
}

int main(int argc, char *argv[])
{
	int i = 0;
	unsigned int retry;

	unsigned int currentBalance, amount, reward;
	pid_t myPID, userPID, nodePID;

	struct timespec randSleepTime;
	struct timespec sleepTimeRemaining;

	struct sigaction saUSR1;
	struct sigaction saINT;

	/* -- ATTACH IPC OBJECTS -- */
	struct parameters *par = shmat(PARAMETERS, NULL, 0);
	user *usersPID = shmat(USERS_PID_ARRAY, NULL, 0);
	node *nodesPID = shmat(NODES_PID_ARRAY, NULL, 0);

	myPID = getpid();

	/* should give error anyway, pretty sure we don't need this check but
	 * Fabio suggested it to me so it must stay for now
	if (argc == 0)
	{
		printf("Error in USERS: %d, no arguments passed\n", myPID);
		return;
	} */

	/* -- SIGNAL HANDLERS --
	 * first set all bytes of sigation to 0
	 * then initialize sa.handler to a pointer to
	 * the function user_transaction/interrupt_handle
	 * then set the handler to handle SIUSR1/SIGINT signals
	 * ((struct sigaction *oldact) = NULL)
	 */
	bzero(&saUSR1, sizeof(saUSR1));
	bzero(&saINT, sizeof(saINT));
	saUSR1.sa_handler = user_transactions_handle;
	saINT.sa_handler = user_interrupt_handle;
	sigaction(SIGUSR1, &saUSR1, NULL);
	sigaction(SIGINT, &saINT, NULL);

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

		currentBalance = 100 /*balance(myPID)*/;
		if (currentBalance >= 2)
		{
			userPID = get_random_userPID(usersPID);
			nodePID = get_random_nodePID(nodesPID);

			amount = RAND(2, currentBalance);

			reward = REWARD(amount, par->SO_REWARD);
			amount -= reward;

			/* need a message queue to enable user to communicate with node
			 * and send it a send_transaction request
			 */
			send_transaction(myPID, userPID, amount, reward);

			/* if (aborted) /* pseudo code, node needs to send an aborted or accepted signal*
				retry--;
			else
				retry = SO_RETRY; */
			if (retry == 0)
				return MAX_RETRY;

			clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &randSleepTime, &sleepTimeRemaining);
			clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTimeRemaining, NULL);
		}
		else
		{
			if (i == 0)
			{
				printf("User %d went broke :/\n", myPID);
				i = 1;
			}

			/* update_status(WENT_BROKE); ////////
			wait_for_incoming_transaction(); ///////// */
		}
	}
}
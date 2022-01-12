#include "include/common.h"
#include "include/users.h"

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

/*
 ======================
 || GLOBAL VARIABLES ||
 ======================
 */

/* parameters of simulation */
struct parameters *par;
user *usersPID;
node *nodesPID;
ledger *mainLedger;

int semID;

pid_t myPID;

/*
 ======================
 ||    FUNCTIONS     ||
 ======================
 */

/* returns index of where user with PID_toSearch is located in usersPID[] */
int get_pid_userIndex(int PID_toSearch)
{
	int i;

	for (i = 0; i < par->SO_USER_NUM; i++)
	{
		if (usersPID[i].pid == myPID)
			return i;
	}

	return -1;
}

/* returns a random PID of a non-dead user from usersPID[] */
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

/* returns a random PID of an available node from nodesPID[] */
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

/* safely updates status of user to statusToSet: 0 alive, 1 broke, 2 dead */
void update_status(int statusToSet)
{
	int i = get_pid_userIndex(myPID);
	if (i == -1)
	{
		TRACE((":users: %d failed to find myself in usersPID[]", myPID));
	}

	sem_reserve(semID, 1);
	usersPID[i].status = statusToSet;
	if (statusToSet == 2)
	{
		/*usersPrematurelyDead++;*/
		TRACE((":users: dead increased\n"));
	}
	sem_release(semID, 1);
}

/* attaches ipc objects based on IDs passed via arguments */
void attach_ipc_objects(char **argv)
{
	par = shmat(PARAMETERS_ARGV, NULL, 0);
	TEST_ERROR
	usersPID = shmat(USERS_PID_ARGV, NULL, 0);
	TEST_ERROR
	nodesPID = shmat(NODES_PID_ARGV, NULL, 0);
	TEST_ERROR
	mainLedger = shmat(LEDGER_ARGV, NULL, 0);
	TEST_ERROR
	semID = SEM_ID_ARGV;
	TRACE((":users: %d semID is %d\n", getpid(), semID));
}

/* SIGUSR1 handler, sends a transaction */
void user_transactions_handle(int signum)
{
	/* return send_transaction(myPID, userPID, amount, reward); */
}

/* CTRL-C handler */
void user_interrupt_handle(int signum)
{
	write(1, "::User:: SIGINT ricevuto\n", 26);
	exit(0);
}

int main(int argc, char *argv[])
{
	int i = 0;
	int currentBalance, amount, reward, retry;
	pid_t userPID, nodePID;

	struct timespec randSleepTime;
	struct timespec sleepTimeRemaining;

	struct sigaction saUSR1;
	struct sigaction saINT;

	struct sembuf sops;

	if (argc == 0)
	{
		printf(":users: %d, no arguments passed, can't continue like this any more :C\n", myPID);
		return ERROR;
	}

	srand(time(NULL)); /* initialize rand function */
	myPID = getpid();  /* set myPID value */

	attach_ipc_objects(argv);

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
			{
				update_status(2);
				return MAX_RETRY;
			}

			clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &randSleepTime, &sleepTimeRemaining);
			clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepTimeRemaining, NULL);
		}
		else
		{
			printf(":users: %d went broke :/\n", myPID);
			update_status(1);

			/*wait_for_incoming_transaction(); ///////// */
		}
	}
}
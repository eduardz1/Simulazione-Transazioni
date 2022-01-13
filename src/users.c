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
int queueID;

int currBalance;
pid_t myPID;
int outGoingTransactions; /* accumulate amount of transactions sent but yet to be received */
transaction currTrans;

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
pid_t get_random_userPID()
{
	int index;
	pid_t val = 0;

	do
	{
		index = RAND(0, par->SO_USER_NUM - 1);
		TRACE((":user: %d index is %d\n", myPID, index))
		TRACE((":users: %d usersPID[%d]\n", myPID, index));
		if (usersPID[index].status != dead)
			val = usersPID[index].pid;
	} while (!val);

	return val;
}

/* returns a random PID of an available node from nodesPID[] */
pid_t get_random_nodePID()
{
	int index;
	pid_t val = 0;

	do
	{
		index = RAND(0, par->SO_NODES_NUM - 1);
		TRACE((":user: %d index is %d\n", myPID, index))
		TRACE((":users: %d nodesPID[%d]\n", myPID, index));
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
	TRACE((":users %d par->SO_RETRY %d\n", myPID, par->SO_RETRY))
	TEST_ERROR
	usersPID = shmat(USERS_PID_ARGV, NULL, 0);
	TRACE((":users: %d usersPID[0] = %d, usersPID[3] = %d\n", myPID, usersPID[0], usersPID[3]))
	TEST_ERROR
	nodesPID = shmat(NODES_PID_ARGV, NULL, 0);
	TRACE((":users: %d nodesPID[0] = %d, nodesPID[3] = %d\n", myPID, nodesPID[0], nodesPID[3]))
	TEST_ERROR
	mainLedger = shmat(LEDGER_ARGV, NULL, 0);
	TEST_ERROR
	semID = SEM_ID_ARGV;
	TRACE((":users: %d semID is %d\n", myPID, semID));
}

/* use nodePID as key for msgget and check for errors */
void queue_to_pid(pid_t nodePID)
{
	queueID = msgget(nodePID, IPC_CREAT | 0600);
	TEST_ERROR
	TRACE((":users: %d -> %d queueID %d\n", myPID, nodePID, queueID))
}

/* initializes transaction values and sets it to pending */
void transaction_init(pid_t userPID, int amount, int reward)
{
	struct timespec exactTime;

	currTrans.sender = myPID;
	currTrans.receiver = userPID;
	currTrans.amount = amount;
	currTrans.reward = reward;
	currTrans.status = pending;
	clock_gettime(CLOCK_REALTIME, &exactTime);
	currTrans.timestamp = exactTime;
}

/* initializes signal handlers for SIGINT and SIGUSR1 */
void signal_handlers_init(struct sigaction *saUSR1, struct sigaction *saINT)
{
	/* -- SIGNAL HANDLERS --
	 * first set all bytes of sigation to 0
	 * then initialize sa.handler to a pointer to
	 * the function user_transaction/interrupt_handle
	 * then set the handler to handle SIUSR1/SIGINT signals
	 * ((struct sigaction *oldact) = NULL)
	 */
	saUSR1->sa_handler = user_transactions_handle;
	saINT->sa_handler = user_interrupt_handle;
	sigaction(SIGUSR1, saUSR1, NULL);
	sigaction(SIGINT, saINT, NULL);
}

/* send transaction currTrans to user userPID via node nodePID */
int send_transaction()
{
	msgsnd(queueID, &currTrans, sizeof(transaction), 0);
	TEST_ERROR
	currBalance -= (currTrans.amount + currTrans.reward);
	outGoingTransactions += (currTrans.amount + currTrans.reward);
	switch (errno)
	{
	case EACCES:
		printf(":users %d no write permission on queue\n", myPID);
		break;
	case EAGAIN:
		printf(":users: %d queue full\n", myPID); /* keep if we decide to use IPC_NOWAIT */
		break;
	case EFAULT:
		printf(":users: %d address pointed by msgp inaccessible\n", myPID);
		break;
	case EIDRM:
		printf(":users: %d message queue removed\n", myPID);
		break;
	case EINTR:
		TRACE((":users: %d signal caught when waiting for queue to free\n", myPID));
		break;
	case EINVAL:
		printf(":users: %d invalid  msqid  value,  or nonpositive mtype value, or invalid msgsz value\n", myPID);
		break;
	case ENOMEM:
		printf(":users: %d system out of memory\n", myPID); /* should basically never happen I hope */
		break;
	default:
		TRACE(("Transaction sent\n"))
		return SUCCESS;
	}
	currTrans.status = aborted;
	currBalance += (currTrans.amount + currTrans.reward);
	outGoingTransactions -= (currTrans.amount + currTrans.reward);
	/* we can then track this type of aborted transactions but rn there's no need to */
	return ERROR;
}

/* SIGUSR1 handler, sends a transaction */
void user_transactions_handle(int signum)
{
	write(1, "::User:: SIGUSR1 received\n", 27);
	if (currBalance > 2)
		send_transaction(); /* we're calling a printf which is not thread safe, need to fix somehow*/
	else
		write(1, "::User:: sorry balance too low\n", 32);
}

/* CTRL-C handler */
void user_interrupt_handle(int signum)
{
	write(1, "::User:: SIGINT received\n", 26);
	exit(0);
}

int main(int argc, char *argv[])
{
	int amount, reward, retry;
	pid_t userPID, nodePID;

	struct timespec randSleepTime;
	struct timespec sleepTimeRemaining;

	struct sembuf sops;
	struct message transMsg;

	struct sigaction saUSR1;
	struct sigaction saINT_user;
	bzero(&saUSR1, sizeof(saUSR1));
	bzero(&saINT_user, sizeof(saINT_user));

	myPID = getpid(); /* set myPID value */
	TRACE((":users: %d USERS_PID_ARGV %d\n", myPID, USERS_PID_ARGV))
	TRACE((":users: %d NODES_PID_ARGV %d\n", myPID, NODES_PID_ARGV))
	TRACE((":users: %d PARAMETERS_ARGV %d\n", myPID, PARAMETERS_ARGV))
	TRACE((":users: %d LEDGER_ARGV %d\n", myPID, LEDGER_ARGV))
	TRACE((":users: %d SEM_ID_PID_ARGV %d\n", myPID, SEM_ID_ARGV))

	if (argc == 0)
	{
		printf(":users: %d, no arguments passed, can't continue like this any more :C\n", myPID);
		return ERROR;
	}

	srand(time(NULL)); /* initialize rand function */

	attach_ipc_objects(argv);
	signal_handlers_init(&saUSR1, &saINT_user);
	transMsg.mtype = atol("transaction");

	retry = par->SO_RETRY;
	while (1)
	{
		SLEEP_TIME_SET
		/*
		 * save the time unslept when interrupted by SIGUSR1
		 * so that we can't force transactions at a much greater speed
		 * better to save it into a separate struct because clock_nanosleep
		 * will not update it if the sleep is not interrupted
		 */
		bzero(&sleepTimeRemaining, sizeof(sleepTimeRemaining));

		currBalance = 100 /*balance(myPID)*/;
		if (currBalance >= 2)
		{
			userPID = get_random_userPID();
			nodePID = get_random_nodePID();

			amount = RAND(2, currBalance);
			reward = REWARD(amount, par->SO_REWARD);
			amount -= reward;

			queue_to_pid(nodePID);
			transaction_init(userPID, amount, reward);
			if (send_transaction() == 0)
				retry = par->SO_RETRY;
			else
				retry--;

			if (retry == 0)
			{
				update_status(2);
				return MAX_RETRY;
			}
			SLEEP
		}
		else
		{
			printf(":users: %d went broke :/\n", myPID);
			update_status(1);

			/*wait_for_incoming_transaction(); ///////// */
		}
	}
}
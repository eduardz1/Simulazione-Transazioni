#include "include/common.h"
#include "include/users.h"
#include "utils/lklist.h"

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
block *ledger;

int semPIDs_ID;
int queueID;

struct node *outGoingTransactions = NULL;

int currBalance;
pid_t myPID;
struct msgbuf_trans transMsg;

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

	return ERROR;
}

/* returns a random PID of a non-dead user from usersPID[] */
pid_t get_random_userPID()
{
	int index;
	pid_t val = 0;

	do
	{
		index = RAND(0, par->SO_USER_NUM - 1);
		if (usersPID[index].status != dead && usersPID[index].pid != myPID)
			val = usersPID[index].pid;
	} while (!val);

	TRACE(("[USER %d] extracted usersPID[%d]\n", myPID, index));
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
		TRACE(("[USER %d] extracted nodesPID[%d]\n", myPID, index));
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
		TRACE(("[USER %d] failed to find myself in usersPID[]", myPID));
	}

	sem_reserve(semPIDs_ID, 1);
	usersPID[i].status = statusToSet;
	if (statusToSet == 2)
	{
		/*usersPrematurelyDead++;*/
		TRACE(("[USERS] dead increased\n"));
	}
	sem_release(semPIDs_ID, 1);
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
	ledger = shmat(LEDGER_ARGV, NULL, 0);
	TEST_ERROR
	semPIDs_ID = SEM_PIDS_ARGV;
}

/* try to attach to queue of nodePID key until it succeeds */
void queue_to_pid(pid_t nodePID)
{
	do
	{
		queueID = msgget(nodePID, 0);
		/* TRACE(("[USER %d] is trying to attach to id=%d queue\n", myPID, queueID)) */
	} while (errno == ENOENT);
	TRACE(("[USER %d] succedeed in attaching to queue %d\n", myPID, queueID))
}

/* initializes transaction values and sets it to pending */
void transaction_init(pid_t userPID, int amount, int reward)
{
	struct timespec exactTime;
	clock_gettime(CLOCK_REALTIME, &exactTime);

	transMsg.mtype = TRANSACTION_MTYPE;
	transMsg.transactionMessage.userTrans.sender = myPID;
	transMsg.transactionMessage.userTrans.receiver = userPID;
	transMsg.transactionMessage.userTrans.amount = amount;
	transMsg.transactionMessage.userTrans.reward = reward;
	transMsg.transactionMessage.userTrans.status = pending;

	transMsg.transactionMessage.userTrans.timestamp = exactTime;
	transMsg.transactionMessage.hops = 0;
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
	/* accumulate amount of transactions sent but yet to be received */
	int out = 0;
	struct node newNode;

	msgsnd(queueID, &transMsg, sizeof(struct msgbuf_trans), 0);
	switch (errno)
	{
	case EACCES:
		printf("[USER %d] no write permission on queue\n", myPID);
		break;
	case EFAULT:
		printf("[USER %d] address pointed by msgp inaccessible\n", myPID);
		break;
	case EIDRM:
		printf("[USER %d] message queue removed\n", myPID);
		break;
	case EINTR:
		TRACE(("[USER %d] signal caught when waiting for queue to free\n", myPID));
		break;
	case EINVAL:
		printf("[USER %d] invalid  msqid  value,  or nonpositive mtype value, or invalid msgsz value\n", myPID);
		break;
	case ENOMEM:
		printf("[USER %d] system out of memory\n", myPID); /* should basically never happen I hope */
		break;
	default:
		TRACE(("[USER %d] sent a transaction of %d UC to [USER %d] via queue %d\n", myPID, transMsg.transactionMessage.userTrans.amount, transMsg.transactionMessage.userTrans.receiver, queueID))
		
		/* track transactions that are yet to be received */
		if(outGoingTransactions == NULL){
			newNode = new_node(&transMsg.transactionMessage.userTrans);
			outGoingTransactions = &newNode;
		} else {
			push(outGoingTransactions, &transMsg.transactionMessage.userTrans);
		}
		return SUCCESS;
	}
	currBalance -= (transMsg.transactionMessage.userTrans.amount + transMsg.transactionMessage.userTrans.reward);
	out += (transMsg.transactionMessage.userTrans.amount + transMsg.transactionMessage.userTrans.reward);

	transMsg.transactionMessage.userTrans.status = aborted;
	currBalance += (transMsg.transactionMessage.userTrans.amount + transMsg.transactionMessage.userTrans.reward);
	out -= (transMsg.transactionMessage.userTrans.amount + transMsg.transactionMessage.userTrans.reward);
	/* we can then track this type of aborted transactions but rn there's no need to */
	return ERROR;
}

/* searches transactionList of blockToSearch and returns total of UC sent and received */
int search_trans_list(block *blockToSearch)
{
	int i, accumulate = 0;

	for (i = 1; i < SO_BLOCK_SIZE - 1; i++)
	{
		if (blockToSearch->transList[i].sender == myPID)
			accumulate -= blockToSearch->transList[i].amount;
		else if (blockToSearch->transList[i].receiver == myPID)
			accumulate += blockToSearch->transList[i].amount;
	}

	return accumulate;
}



/* saves balance of calling user in currBalance */
void get_balance()
{
	int i, j;
	int accumulate = 0;

	for (i = 0; i < SO_REGISTRY_SIZE; i++)
	{
		for (j = 1; j < SO_BLOCK_SIZE - 1; j++)
		{
			find_and_remove(outGoingTransactions, &ledger[i].transList[j]); /* SEGFAULT */
			if (ledger[i].transList[j].sender == myPID)
			{
				accumulate -= ledger[i].transList[j].amount;
			}
			else if (ledger[i].transList[j].receiver == myPID)
			{
				TRACE(("[USER %d] found myself as receiver of %d UC\n", myPID, ledger[i].transList[j].amount))
				accumulate += ledger[i].transList[j].amount;
			}
		}
	}

	currBalance += accumulate;
}

/* SIGUSR1 handler, sends a transaction */
void user_transactions_handle(int signum)
{
	write(1, "::USER:: SIGUSR1 received\n", 27);
	if (currBalance > 2)
		send_transaction(); /* we're calling a printf which is not thread safe, need to fix somehow*/
	else
		write(1, "::USER:: sorry balance too low\n", 32);
}

/* CTRL-C handler */
void user_interrupt_handle(int signum)
{
	write(1, "::USER:: SIGINT received\n", 26);

	report_mem_leak_users();
	exit(0);
}

int main(int argc, char *argv[])
{
	int amount, reward, retry;
	pid_t userPID, nodePID;

	struct timespec randSleepTime;
	struct timespec sleepTimeRemaining;

	struct sigaction saUSR1;
	struct sigaction saINT_user;
	bzero(&saUSR1, sizeof(saUSR1));
	bzero(&saINT_user, sizeof(saINT_user));

	myPID = getpid(); /* set myPID value */

	if (argc == 0)
	{
		printf("[USER %d] no arguments passed, can't continue like this any more :C\n", myPID);
		return ERROR;
	}

	srand(myPID); /* initialize rand function, time(NULL) uses the seconds and the pattern is the same for every user */

	attach_ipc_objects(argv);
	signal_handlers_init(&saUSR1, &saINT_user);
	transaction_pool_init(&outGoingTransactions);
	transMsg.mtype = TRANSACTION_MTYPE;

	currBalance = par->SO_BUDGET_INIT;
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

		get_balance();
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
			/*else
				retry--;

			if (retry == 0)
			{
				update_status(2);
				return MAX_RETRY;
			}*/
			SLEEP
		}
		else
		{
			update_status(1);
			sleep(1);
		}
	}
}
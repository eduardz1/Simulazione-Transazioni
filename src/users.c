#include "include/common.h"
#include "include/users.h"
#include "include/print.h"
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

extern int errno;

/* parameters of simulation */
struct parameters *par;
user *usersPID;
node *nodesPID;
block *ledger;

int semPIDs_ID;
int queueID;

struct node *outGoingTransactions = NULL;

unsigned int currBalance;
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
		errno = 0;
		queueID = msgget(nodePID, 0);
	} while (errno == ENOENT);
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
	transMsg.transactionMessage.hops = par->SO_HOPS;
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
	transaction sent;

	TRACE(("[USER %d] queueID is %d\n", myPID, queueID))
	if (send_message(queueID, &transMsg, sizeof(struct msgbuf_trans), IPC_NOWAIT) == 0)
	{

		TRACE(("[USER %d] sent a transaction of %d UC to [USER %d] via queue %d\n", myPID, transMsg.transactionMessage.userTrans.amount, transMsg.transactionMessage.userTrans.receiver, queueID))
		currBalance -= (transMsg.transactionMessage.userTrans.amount + transMsg.transactionMessage.userTrans.reward);

		sent = transMsg.transactionMessage.userTrans;
		/* track transactions that are yet to be received */
		if (outGoingTransactions == NULL)
			outGoingTransactions = new_node(sent);
		else
			push(outGoingTransactions, sent);

		return SUCCESS;
	}

	transMsg.transactionMessage.userTrans.status = aborted;
	currBalance += (transMsg.transactionMessage.userTrans.amount + transMsg.transactionMessage.userTrans.reward);

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

/* safely updates balance of user */
void update_balance(unsigned int tempBalance)
{
	int i = get_pid_userIndex(myPID);

	sem_reserve(semPIDs_ID, 1);
	currBalance = tempBalance;
	usersPID[i].balance = currBalance;
	sem_release(semPIDs_ID, 1);
}

/* saves balance of calling user in currBalance */
void get_balance()
{
	struct node *tmp;
	int i, j;
	long accumulate = 0; /* needs to fit two unsigned ints inside */
	long flag = 1;

	/* balance is buffere in tempBalance so that if the program is interrupted
	 * while get_balance() is running the user doesn't suddently get his
	 * balance reset to SO_BUDGET_INIT
	 */
	unsigned int tempBalance = par->SO_BUDGET_INIT;

	/* create a local copy to avoid inconsistencies, we could use semaphores
	 * but parsing the entire ledger and the outGoing list takes an awful lot
	 * and would make our program very inefficient, memory is relatively cheap,
	 * even with real blockchains you have to save the entire chain to the hard drive
	 * if you want to make a local keychain so it should make sense
	 */
	block ledgerTemp[SO_REGISTRY_SIZE];
	memcpy(&ledgerTemp, ledger, sizeof(ledgerTemp));

	for (i = 0; i < SO_REGISTRY_SIZE && flag != 0; i++)
	{
		/* can't have time = 0 unless the block is not initialized */
		flag = (ledgerTemp[i].transList->timestamp.tv_nsec) + (ledgerTemp[i].transList->timestamp.tv_sec);

		for (j = 1; j < SO_BLOCK_SIZE && flag != 0; j++)
		{
			/*TEST_ERROR file too large an absurd amount of times */
			if (ledgerTemp[i].transList[j].sender == myPID)
			{
				find_and_remove(&outGoingTransactions, &ledgerTemp[i].transList[j]);
				accumulate -= (ledgerTemp[i].transList[j].amount + ledgerTemp[i].transList[j].reward);
			}
			else if (ledgerTemp[i].transList[j].receiver == myPID)
			{
				/* TRACE(("[USER %d] found myself as receiver of %d UC\n", myPID, ledgerTemp[i].transList[j].amount)) */
				accumulate += ledgerTemp[i].transList[j].amount;
			}
		}
	}

	tmp = outGoingTransactions;

	/*if (tmp != NULL) /* just for testing, can be removed later *
	{
		TRACE(("[USER %d] accumulate before removing out=%d\n", myPID, accumulate))
		TRACE(("[USER %d] tmp->next: %p\n", myPID, tmp->next))
		if (tmp->next != NULL)
		{
			TRACE(("[USER %d] outGoingTransactions->next: %p\n", myPID, outGoingTransactions->next))
			TRACE(("[USER %d] outGoingTransactions->next->trans = amount: %u, sender: %d, receiver: %d\n", myPID, outGoingTransactions->next->trans.amount, outGoingTransactions->next->trans.sender, outGoingTransactions->next->trans.receiver))
		}
	}*/

	while (tmp != NULL)
	{
		accumulate -= (tmp->trans.amount + tmp->trans.reward);
		tmp = tmp->next;
	}

	TRACE(("[USER %d] accumulate=%d\n", myPID, accumulate))
	if (accumulate * (-1) > tempBalance)
	{
		printf("*** [USER %d] errror in calculating balance, overflow ***\n", myPID);
		print_outgoing_pool(outGoingTransactions);
		killpg(0, SIGINT);
	}

	tempBalance += accumulate;
	if (errno == ERANGE) /* not working as intended */
	{
		printf("[USER %d] went out of bound, punishment for being that rich is death\n", myPID);
		update_status(2);
		kill(myPID, SIGINT);
	}

	update_balance(tempBalance);
}

/* SIGUSR1 handler, sends a transaction */
void user_transactions_handle(int signum)
{
	int retry = par->SO_RETRY;
	write(1, "::USER:: SIGUSR1 received\n", 27);

	if (currBalance > 2)
	{
		pid_t userPID = get_random_userPID();
		pid_t nodePID = get_random_nodePID();

		unsigned int amount = RAND(2, currBalance);
		unsigned int reward = REWARD(amount, par->SO_REWARD);
		amount -= reward;

		update_status(0);

		queue_to_pid(nodePID);
		transaction_init(userPID, amount, reward);
		if (send_transaction() == 0)
			retry = par->SO_RETRY;
		else
			retry--;

		if (retry == 0)
		{
			update_status(2);
			TRACE(("[USER %d] max retry reached, life has no meaning any more\n", myPID))
			kill(myPID, SIGINT);
		}
	}
	else
	{
		write(1, "::USER:: sorry balance too low\n", 32);
	}
}

/* CTRL-C handler */
void user_interrupt_handle(int signum)
{
	write(2, "::USER:: SIGINT received\n", 26);
	get_balance();
	if (currBalance >= 2)
		update_status(0);
	exit(0);
}

int main(int argc, char *argv[])
{
	unsigned int amount, reward, retry;
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
		TRACE(("[USER %d] curr balance: %u\n", myPID, currBalance))
		if (currBalance >= 2)
		{
			update_status(0);

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
				TRACE(("[USER %d] max retry reached, life has no meaning any more\n", myPID))
				kill(myPID, SIGINT);
			}

			SLEEP
		}
		else
		{
			update_status(1);
			sleep(2);
		}
	}
}
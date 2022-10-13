#include "include/common.h"
#include "include/users.h"
#include "include/print.h"
#include "utils/lklist.h"

#define REWARD(amount, reward) (ceil(((reward * (amount)) / 100.0)))

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

int semUsersPIDs_ID;
int semLedger_ID;
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
	unsigned int i;

	for (i = 0; i < par->SO_USER_NUM; i++)
	{
		if (usersPID[i].pid == PID_toSearch)
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
		index = RAND(0, par->SO_NODES_NUM * 2 - 1);
		if (nodesPID[index].status == available && nodesPID[index].pid != 0)
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
		TRACE(("[USER %d] failed to find myself in usersPID[]", myPID))
	}

	sem_reserve(semUsersPIDs_ID, 1);
	usersPID[i].status = statusToSet;
	if (statusToSet == 2)
	{
		TRACE(("[USERS] dead increased\n"))
	}
	sem_release(semUsersPIDs_ID, 1);
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
	semUsersPIDs_ID = SEM_USERS_PIDS_ARGV;
	semLedger_ID = SEM_LEDGER_ARGV;
}

/* try to attach to queue of nodePID key until it succeeds 
void queue_to_pid(pid_t nodePID)
{
	do
	{
		errno = 0;
		queueID = msgget(nodePID, 0);
	} while (errno == ENOENT);
}
*/

/* initializes transaction values and sets it to pending */
void transaction_init(pid_t userPID, int amount, int reward)
{
	struct timespec exactTime = {0};
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
	transaction sent = {0};
	key_t key;
	key=ftok("key.txt",'100'); 
	transMsg.mtype=1;
	queueID=msgget(key,IPC_CREAT|0600);

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

	sem_reserve(semUsersPIDs_ID, 1);
	currBalance = tempBalance;
	usersPID[i].balance = currBalance;
	sem_release(semUsersPIDs_ID, 1);
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
	sem_reserve(semLedger_ID, 1);
	memcpy(&ledgerTemp, ledger, sizeof(ledgerTemp));
	sem_release(semLedger_ID, 1);

	for (i = 0; i < SO_REGISTRY_SIZE && flag != 0; i++)
	{
		/* can't have time = 0 unless the block is not initialized */
		flag = (ledgerTemp[i].transList->timestamp.tv_nsec) + (ledgerTemp[i].transList->timestamp.tv_sec);

		for (j = 1; j < SO_BLOCK_SIZE && flag != 0; j++)
		{
			if (ledgerTemp[i].transList[j].sender == myPID)
			{
				find_and_remove(&outGoingTransactions, &ledgerTemp[i].transList[j]);
				accumulate -= (ledgerTemp[i].transList[j].amount + ledgerTemp[i].transList[j].reward);
			}
			else if (ledgerTemp[i].transList[j].receiver == myPID)
			{
				accumulate += ledgerTemp[i].transList[j].amount;
			}
		}
	}

	tmp = outGoingTransactions;

	while (tmp != NULL)
	{
		accumulate -= (tmp->trans.amount + tmp->trans.reward);
		tmp = tmp->next;
	}

	if (accumulate * (-1) > tempBalance)
	{
		fprintf(stderr, "*** [USER %d] errror in calculating balance, overflow ***\n", myPID);
		update_status(2);
		print_outgoing_pool(outGoingTransactions, (FILE *)2);
		killpg(0, SIGINT);
	}

	tempBalance += accumulate;
	if (errno == ERANGE) /* not working as intended */
	{
		fprintf(stderr, "[USER %d] went out of bound, punishment for being that rich is death\n", myPID);
		update_status(2);
		kill(myPID, SIGINT);
	}

	update_balance(tempBalance);
}

/* SIGUSR1 handler, sends a transaction */
void user_transactions_handle(int signum)
{
	int retry = par->SO_RETRY;
	/* cast return value into the void, ! is needed because of gcc behaviour */
	(void)!write(1, "::USER:: SIGUSR1 received\n", 27);

	get_balance();
	if (currBalance >= 2)
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
		/* cast return value into the void, ! is needed because of gcc behaviour */
		(void)!write(1, "::USER:: sorry balance too low\n", 32);
	}
}

/* CTRL-C handler */
void user_interrupt_handle(int signum)
{
	struct node *tmp;
	int i = get_pid_userIndex(myPID);

#ifdef DEBUG
	/* cast return value into the void, ! is needed because of gcc behaviour */
	(void)!write(2, "::USER:: SIGINT received\n", 25);
#endif

	get_balance();

	if (usersPID[get_pid_userIndex(myPID)].status != dead)
	{
		if (usersPID[i].balance >= 2)
			update_status(0);
		else
			update_status(1);
	}

	tmp = outGoingTransactions;
	sem_reserve(semUsersPIDs_ID, 1);
	while (tmp != NULL)
	{
		usersPID[i].balance += (tmp->trans.amount + tmp->trans.reward);
		tmp = tmp->next;
	}
	sem_release(semUsersPIDs_ID, 1);

	free(tmp);
	exit(0);
}

int main(int argc, char *argv[])
{
	unsigned int amount, reward, retry;
	pid_t userPID, nodePID;

	struct timespec randSleepTime = {0};
	struct timespec sleepTimeRemaining = {0};

	struct sigaction saUSR1;
	struct sigaction saINT_user;
	BZERO(&saUSR1, sizeof(saUSR1));
	BZERO(&saINT_user, sizeof(saINT_user));

	myPID = getpid(); /* set myPID value */

	if (argc == 0)
	{
		printf("[USER %d] no arguments passed, can't continue like this any more :C\n", myPID);
		update_status(2);
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
		}
	}
}
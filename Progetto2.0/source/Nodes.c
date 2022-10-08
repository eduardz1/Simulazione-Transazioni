#include "include/Nodes.h"

/*#define SENDER -1*/
/*struct mesg_buffer *MessageQ;*/
pool transPool;
Message message;
Message *newTransaction;
Message *Trans_ptr;
Message fetchMex;
pid_t *friendList;
Block_ *ledger;
#define BUFF_MAX 10
/*POINTER TO STRUCTURE  */
user *UserID;
node_t *NodeID;
Block_ *Ledger;

/* -------------*/
int Msg_ID;
int Message_Erro;
int Money_q;
int toend;
pid_t myPID;
pid_t nPid;
key_t MSG_Key;
int friendList_size;
int sem_id_ledger;
int semNodesPIDs_ID;

void take_transaction()
{
	unsigned int friendCycle = 20;
	friend_msg friends_recived;
	int sizeofFriend = friendList_size;
	if (transPool.size < SO_TP_SIZE && friendCycle < 20)
	{
		if (receive_message(Msg_ID, &fetchMex, sizeof(Message), message.m_type, 0) == 0) /*undefined reference to receive_message() dunno why*/
		{
			add_to_pool(&transPool, &fetchMex);
			transPool.size++;
		}

		if (receive_message(Msg_ID, &friends_recived, sizeof(Message), message.m_type, IPC_NOWAIT) == 0)
		{
			friendList = realloc(friendList, sizeof(pid_t) * (friendList_size + 1));
			friendList++;
			friendList[sizeofFriend] = friends_recived.friend;
		}

		friendCycle++;
	}
	else
	{
		/* Random friends da implemmentare funzione */
		friendCycle = 0;
	}
}
void Block(transaction *blockT, Block_ *newBlock)
{
	int i;
	transaction reward;
	struct timespec time;
	bzero(&time, sizeof(time));
	clock_gettime(CLOCK_REALTIME, &time);
	reward.Sender = NO_BLOCK; /* Macro defined in common.h*/
	reward.time = time;
	reward.Receiver = myPID;
	reward.Reward = 0;
	reward.MoneyStatusTrans = pending;
	reward.Money = sum_reward(blockT);
	newBlock->t_list[0] = reward;

	for (i = 1; i < SO_BLOCK_SIZE; i++)
	{
		newBlock->t_list[i] = blockT[i - 1];
	}
}
void transListTo_block(transaction *Noreward)
{
	int i;
	Message tmp;
	bzero(&tmp, sizeof(tmp));
	for (i = 0; i < (SO_BLOCK_SIZE - 1); i++)
	{
		if (remove_from_pool(&transPool, &tmp) == PROBLEM)
		{
			printf("Problem in NODE n * %d\n", myPID);
			kill(myPID, SIGINT);
			exit(EXIT_FAILURE);
		}
		Noreward[i] = tmp.Message_Transaction.uTrans;

		transPool.size--;
	}
}

void fill_friends(pid_t *friendList)
{
	unsigned int i;
	friend_msg friendMex;

	bzero(&friendMex, sizeof(friendMex)); /* azzero i byte in memoria */
	for (i = 0; i < SO_FRIENDS_NUM; i++)
	{
		receive_message(Msg_ID, &friendMex, sizeof(friend_msg), friendMex.mtype, 0);
		friendList[i] = friendMex.friend;
	}
}

void confirm_state_block(Block_ *toConfirm)
{
	int i;
	for (i = 0; i < SO_BLOCK_SIZE; i++)
	{
		toConfirm->t_list[i].MoneyStatusTrans = confirmed;
	}
}

void block_ledger(Block_ *newBlock)
{
	int i;
	Block_ tmp;
	for (i = 0; SO_REGISTRY_SIZE; i++)
	{
		if (ledger[i].t_list[1].Sender == 0)
		{
			/* Casto nel blocco tmp un nuovo nodo con tutte le sue caratteristiche */
			tmp = *newBlock;
			tmp.blockIndex = 1;
			/* preleviamo le risorse per la creazione del ledger */
			resource_set(sem_id_ledger, 1);
			ledger[i] = tmp;
			confirm_state_block(&ledger[i]);
			resource_release(sem_id_ledger, 1);
			return;
		}
	}
	printf("NODE n * %d ledger is full i can t insert block \n", myPID);
	killpg(0, SIGINT); /* invia segnali ad un gruppo di process almeno questo ho capito */
}

/*https://www.geeksforgeeks.org/ipc-using-message-queues*/
int sum_reward(transaction *sumBlock)
{
	int i = 0;
	int sum = 0;

	for (i = 0; i < (SO_BLOCK_SIZE - 1); i++)
	{
		sum += sumBlock[i].Reward;
	}
	return sum;
}

void ipc_Attach_argv(char **argv)
{
	UserID = shmat(USERS_PID_ARGV, NULL, 0);
	NodeID = shmat(NODES_PID_ARGV, NULL, 0);
	Ledger = shmat(LEDGER_ARGV, NULL, 0);
	/*MAncano i semafori  */
}
int get_pid_node_index()
{
	unsigned int i;
	i = 0;
	for (i = 0; i < SO_NODES_NUM * 2; i++)
	{
		if (NodeID[i].nodPid == myPID)
		{
			return i;
		}
	}
	return PROBLEM;
}

void sig_handler_init(struct sigaction *sa)
{
	sa->sa_handler = node_handler_interrupt;
	sigaction(SIGINT, sa, NULL);
}

void node_handler_interrupt(int sigum)
{
	int i;
	int accurate_balance;
	accurate_balance = 0;

	msgctl(Msg_ID, IPC_RMID, NULL);
	/* travers ledger because only the balance written on ledger at the end is
	 * the one that should be displayed */

	for (i = 0; i < SO_REGISTRY_SIZE; i++)
	{
		if (Ledger[i].t_list[0].Receiver == myPID)
		{
			accurate_balance += Ledger[i].t_list[0].Money;
		}
	}
	resource_set(semNodesPIDs_ID, 1);
	NodeID[get_pid_node_index()].balance = accurate_balance;
	resource_release(semNodesPIDs_ID, 1);

	exit(0);
}

/*transaction pool is a linked list that contains struct of message with all information (see Common header)
 * to understand free and malloc look up to composite_data_type structure last slide
 * */
void transaction_pool_init(pool *transPool)
{
	transPool->head = NULL;
	transPool->tail = NULL;
	transPool->size = 0;
}

int add_to_pool(pool *transPool, Message *message)
{
	newTransaction = malloc(sizeof(newTransaction));
	if (newTransaction == NULL)
	{
		perror("MALLOC ERROR IN POOL\n ");
		exit(EXIT_FAILURE);
	}
	while (newTransaction != NULL)
	{
		newTransaction->Message_Transaction = message->Message_Transaction;
		newTransaction->Message_Transaction.next = NULL;

		if (transPool->tail)
		{
			transPool->head = newTransaction;
		}
	}
	return SUCCESS;
}

int remove_from_pool(pool *transPool, Message *message_t)
{
	if (transPool == NULL)
	{
		return ERROR;
	}
	else if (transPool->head == NULL)
	{
		return ERROR;
	}
	message_t = transPool->head;
	free(transPool->head);
	transPool->head = transPool->head->Message_Transaction.next;

	if (transPool->head == NULL)
	{
		transPool->tail = NULL;
	}
	return SUCCESS;
}

void message_queue_attach()
{
	do
	{
		Msg_ID = msgget(myPID, 0);
	} while (errno == ENOENT);
}

void Message_Rec(int messageID, key_t messageKey)
{
	MSG_Key = &nPid;
	Msg_ID = msgget(MSG_Key, 0666 | IPC_CREAT);
	msgrcv(Msg_ID, Trans_ptr, sizeof(Trans_ptr->Message_Transaction), 1, 0);
	printf("DATA RECIVED : %s \n", Trans_ptr->mesText);
	msgctl(Msg_ID, IPC_RMID, NULL);
}


void node_handler(int signum)
{
	int i;
	int accurate_balance = 0;
	printf("[NODE] Sigint received\n");
	msgctl(Msg_ID, IPC_RMID, NULL);

	for(i=0;i<SO_REGISTRY_SIZE;i++){
		if(ledger[i].t_list[0].Receiver == myPID){
			accurate_balance += ledger[i].t_list[0].Money;
	}
	resource_set(semNodesPIDs_ID,1);
	NodeID[get_pid_node_index()].balance = accurate_balance;
	resource_release(semNodesPIDs_ID,1);

	exit(0);
}
}


void signal_handler_init(struct sigaction *saInt_node)
{
	saInt_node->sa_handler = node_handler;
	sigaction(SIGINT, saInt_node, NULL);
}

int main(int argc, char *argv[])
{
	transaction transBuffer[sizeof(transaction) * (SO_BLOCK_SIZE - 1)]; /*Bufferizziamo in dimensione della block size e della transazione che dobbiamo inserire*/

	struct timespec randSleeptime;
	struct timespec sleeptimeremaning;
	struct sigaction *sa;

	bzero(&randSleeptime, sizeof(randSleeptime));
	bzero(&sleeptimeremaning, sizeof(sleeptimeremaning));
	bzero(&sa, sizeof(sa));

	myPID = getpid();

	if (argc == 0)
	{
		perror("NO ARGUMENT PASSED CHECK IT PLEASE \n ");
		exit(EXIT_FAILURE);
	}
	ipc_Attach_argv(argv);
	srand(getpid());
	sig_handler_init(sa);
	message_queue_attach();

	friendList = malloc(SO_FRIENDS_NUM * sizeof(pid_t));
	friendList_size = SO_FRIENDS_NUM;
	fill_friends(friendList);

	transaction_pool_init(&transPool);
	while (1)
	{
		take_transaction();
		resource_set(semNodesPIDs_ID, 1);
		NodeID[get_pid_node_index()].tpsize = transPool.size;
		resource_release(semNodesPIDs_ID, 1);

		if (transPool.size >= (SO_BLOCK_SIZE - 1))
		{
			transListTo_block(transBuffer);
			switch (fork())
			{
			case -1: /*ERROR CASE*/
				printf("ERROR DURING FORKING TO CREATE A BLOCK n*[--%d--]CHECK IT ", myPID);
				exit(EXIT_FAILURE);
				break;
			case 0: /* child creates a new block and appends it to ledger */
			{
				Block_ *newBlock = malloc(sizeof(Block_));
				/*SLEEP_TIME_SET;*/
				sleep(1);
				Block(transBuffer, newBlock);
				block_ledger(newBlock);
				free(newBlock);
				/*SLEEP;*/
				sleep(1);
				exit(0);
				break;
			}
			default: /*Parent case */
				break;
			}
		}
	}
}

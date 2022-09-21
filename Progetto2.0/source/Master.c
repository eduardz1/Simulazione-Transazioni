#include "include/Master.h"

#define USER_NAME "./Users"
#define NODE_NAME "./Nodes"

user *usersPid;
node *nodesPid;
Block_ ledger[SO_REGISTRY_SIZE];
Block_ *tmpLedger;
int mQueue;

void sems_init(){
	semUsersPid_Id=semget(,1,IPC_CREAT|0666);


}

void create_arguments(int *IPC_array, char **argv)
{
	char uPid_array[13] = {0};
	char nPid_array[13] = {0};
	char ledger[13] = {0};
	char semUserPid_Id[13] = {0};
	char semNodePid_Id[13] = {0};
	char semLedger_Id[13] = {0};

	snprintf(uPid_array, 13, "%d", IPC_array[0]);
	snprintf(nPid_array, 13, "%d", IPC_array[1]);
	snprintf(ledger, 13, "%d", IPC_array[3]);
	snprintf(semUserPid_Id, 13, "%d", IPC_array[4]);
	snprintf(semNodePid_Id, 13, "%d", IPC_array[6]);
	snprintf(semLedger_Id, 13, "%d", IPC_array[5]);

	strncpy(argv[1], uPid_array, 13);
	strncpy(argv[2], nPid_array, 13);
	strncpy(argv[4], ledger, 13);
	strncpy(argv[5], semUserPid_Id, 13);
	strncpy(argv[6], semLedger_Id, 13);
	strncpy(argv[7], semNodePid_Id, 13);
	argv[8] = NULL; /* Terminating argv with NULL value */
}

int message_queue_id()
{
	int pidGot = getpid();
	int qId = msgget(pidGot, IPC_CREAT | IPC_EXCL | 0666);

	switch (errno)
	{
	case EIDRM:
		printf("[PROCESS %d] queue %d was removed\n", pidGot, qId);
		break;
	case EINVAL:
		printf("[PROCESS %d] queue %d invalid value for cmd or msqid\n", pidGot, qId);
		break;
	case EPERM:
		printf("[PROCESS %d] queue %d the effective user ID of the calling process is not the creator or the owner\n", pidGot, qId);
		break;
	}
	return qId;
}

void Sh_MemMaster(key_t key, size_t size, int shmflg)
{
	int m_id;
	int ShInit;
	char *shm;
	int shmdet;
	ShInit = shmget(key, sizeof(SO_REGISTRY_SIZE) * 2, IPC_CREAT | 0666); /*ShdMem Define Area*/ /* Raddoppio l'area per evitare saturazioni*/
	shm = shmat(ShInit, NULL, 0);																																 /*Attach ShMem;*/
	shmdet = shmdt(ShInit);																																			 /*Detach ShMem*/
	system("./Users");
}

void Sh_UserPID(key_t key, size_t size, int shmflg)
{
	key = getpid();
	int Sh_UserPIDDet;
	int Sh_UserPIDInit;
	int id;
	char *Sh_UserPIDAttach;
	Sh_UserPIDInit = shmget(key, sizeof(SO_USERS_NUM), IPC_CREAT | 0666);
	Sh_UserPIDAttach = shmat(Sh_UserPID, NULL, 0);
	Sh_UserPIDDet = shmdt(Sh_UserPIDInit);
}

void Shared_Memory(key_t key, size_t size, int shmflg)
{
	int m_id;
	int *shm;
	int ShInit;
	ShInit = shmget(key, sizeof(SO_REGISTRY_SIZE) * 2, IPC_CREAT | 0666); /*ShdMem Define Area*/
	shm = shmat(ShInit, NULL, 0);																					/*Attach ShMem;*/
	int shmdt(const void *shmaddr);																				/*Detach ShMem*/
}

/* generate the user with fork and lauch ./users with execve*/
void generate_user(int uCounter)
{ /*need to implement uCounter !! */
	pid_t uPid = fork();

	switch (uPid)
	{
	case -1:
		printf("Error in fork for user\n");
		break;
	case 0:
		printf("[PROCESS %d] Forked child %d\n", getpid(), getpid());
		system(USER_NAME);
		break;
	default:
		usersPid[uCounter].usPid = uPid;
		return;
	}
}
int generate_node(int nCounter)
{
	static unsigned int buff = 0;
	pid_t nPid = fork();

	if (buff == 0)
		buff = SO_NODES_NUM;

	if (buff >= SO_NODES_NUM * 2)
		return -1; /*error*/
	else if (nCounter == -1)
		nCounter = buff++;

	switch (nPid)
	{
	case -1:
		printf("ERROR FORKING NODE\n");
		break;

	case 0:
		printf("[PROCESS %d] Forked child %d\n", getpid(), getpid());
		message_queue_id();
		system(NODE_NAME);
		break;

	default:
		nodesPid[nCounter].nodPid = nPid;
		break;
	}
	return nPid;
}

/* Stop Simulation handler Ctrl-C */
void signal_handler(int signum)
{
	char c;
	signal(signum, SIG_IGN);
	printf("you pressed CTRL-C\n"
				 "would you like to quit?[y/n]");
	c = getchar();
	if (c == 'y' || c == 'Y')
	{
		exit(0);
	}
	else
	{
		/*  signal(SIGINT,signal_handler);*/
		getchar();
	}
}

int main()
{
	unsigned int i;
	unsigned int uCounter;
	unsigned int nCounter;
	char *argvCreator[8];
	struct sigaction sa;
	struct sembuf sops;
	for(i=0;i<8;i++)
		argvCreator[i] = malloc(3*sizeof(int)+1);
	
	tmpLedger = ledger;
	sems_init();
	/*signal handler function
	 * set all sigaction's byte to zero
	 * sa.handler to handle signal_handler(),then the handler is set to handle SIGINT signals
	 */

	bzero(&sa, sizeof(sa));
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGALRM, &sa, NULL);
	mQueue = message_queue_id();
	for (nCounter = 0; nCounter < SO_NODES_NUM - 1; nCounter++) /*TODO: seg fault here imo, need to solve */
	{
		nodesPid[nCounter].balance = 0;
		nodesPid[nCounter].Node_state = available;
		nodesPid[nCounter].nodPid = generate_node(nCounter);
	}

	for (uCounter = 0; uCounter < SO_USERS_NUM; uCounter++)
	{
		usersPid[uCounter].Us_state = ALIVE;
		usersPid[uCounter].balance = 0;
		generate_user(uCounter);
	}

	alarm(SO_SIM_SEC);

	switch (fork())
	{
	case -1:
		fprintf(stderr, "[master] fork error\n");
		exit(EXIT_FAILURE);
		break;
	case 0:
	{
		unsigned int i;
		unsigned int tmpPid;

		friend_msg newNode;
		Message transHop;
		bzero(&newNode, sizeof(newNode)); /*set memory region to zero */
		bzero(&transHop, sizeof(transHop));
		signal(SIGINT, SIG_DFL);

		while (1)
		{
			receive_message(mQueue, &newNode, sizeof(Message), TRANSACTION_MTYPE, 0);
			nodesPid[nCounter].Node_state = ALIVE;
			nodesPid[nCounter].balance = 0;
			tmpPid = generate_node(nCounter);
		}
	}
	default:
		break;
	}
	return 0;
}

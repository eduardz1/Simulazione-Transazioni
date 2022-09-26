#include "include/Master.h"

#define USER_NAME "./Users"
#define NODE_NAME "./Nodes"

user *usersPid;
node *nodesPid;
Block_ ledger[SO_REGISTRY_SIZE];
Block_ *tmpLedger;
int mQueue;
int semUsersPid_Id;
int semNodesPid_Id;
int semLedger_Id;


void sems_init()
{

	semUsersPid_Id=semget(SEM_USERS_PIDS_KEY,1,0600|IPC_CREAT|IPC_EXCL);
	semNodesPid_Id=semget(SEM_NODES_PIDS_KEY,1,0600|IPC_CREAT|IPC_EXCL);
	semLedger_Id=semget(SEM_LEDGER_KEY,1,0600|IPC_CREAT|IPC_EXCL);

	switch(errno){
		case EEXIST:
			printf("[MASTER] one or more sems couldn't be created\n");
			exit(1);
		case ENOSPC:
			printf("[MASTER] too many sems in the system");
			exit(1);

		default:
		printf("[MASTER] semUsersPid_id is %d\n",semUsersPid_Id);
		printf("[MASTER] semNodesPid_id is %d\n",semNodesPid_Id);
		printf("[MASTER] semLedger_id is %d\n",semLedger_Id);
		break;

	}

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
	int Sh_UserPIDDet;
	int Sh_UserPIDInit;
	int id;
	char *Sh_UserPIDAttach;
	key = getpid();
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
void generate_user(int uCounter,char *userArgv[])
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
int generate_node(int nCounter,char *nodeArgv[])
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
	printf("Master PID: %d\n", getpid());
	printf("before sems_init\n"); /*TODO: remove,debug only*/
	sems_init();
	printf("after sems_init\n"); /*TODO: remove,debug only*/

	

	 /* set all sigaction's byte to zero
	  sa.handler to handle signal_handler(),then the handler is set to handle SIGINT signals
	 
	 */

	bzero(&sa, sizeof(sa));
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGALRM, &sa, NULL);
	mQueue = message_queue_id();
	printf("before for loop\n");/*TODO: remove,debug only*/
	for (nCounter = 0; nCounter < SO_NODES_NUM; nCounter++) /*TODO: seg fault here imo, need to solve */
	{
		printf("nCounter: %d\n", nCounter);
		printf("in for loop\n");/*TODO: remove,debug only*/
		nodesPid[nCounter].balance = 0; /*TODO seg fault here */
		nodesPid[nCounter].Node_state = available;
		generate_node(nCounter,argvCreator);
	}


	for (uCounter = 0; uCounter < SO_USERS_NUM; uCounter++)
	{
		usersPid[uCounter].Us_state = ALIVE;
		usersPid[uCounter].balance = 0;
		generate_user(uCounter,argvCreator);
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
			tmpPid = generate_node(nCounter,argvCreator);
		}
	}
	default:
		break;
	}
	return 0;
}
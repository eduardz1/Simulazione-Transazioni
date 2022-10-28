#include "include/Master.h"

#define USER_NAME "./Users"
#define NODE_NAME "./Nodes"
#define SHM_NUM 4
#define SEM_NUM 3
#define IPC_NUM SHM_NUM + SEM_NUM
/*-----------------*/
user* usersPid;
node_t *nodesPid;
Block_ ledger[SO_REGISTRY_SIZE];
Block_* tmpLedger;
Block_ *Ledger;
struct msqid_dist *MessageQ;
int mQueue;

int semUsersPid_Id;
int semNodesPid_Id;
int semLedger_Id;
void shared_memory_objects_init(int *shmArray)
{
    /* shared memory segments IDs */
    int usersPID_ID;
    int nodesPID_ID;
    int ledger_ID;
    int par_ID;

    /* parameters init and read from conf file 
    par_ID = shmget(SHM_PARAMETERS, sizeof(par), 0600 | IPC_CREAT | IPC_EXCL);
    par = (struct parameters *)shmat(par_ID, NULL, 0);
    if (parse_parameters(par) == CONF_ERROR)
    {
        TRACE(("*** Error reading conf file, defaulting to conf#1 ***\n"))
    }
    else
    {
        TRACE(("[MASTER] conf file read successful\n"))
    }
	*/

    /* make node array twice as big to account for extra nodes when transactions
     * have hopped out avoiding reallocating a shared memory resource
     */
    nodesPID_ID = shmget(SHM_NODES_ARRAY, (SO_NODES_NUM) * sizeof(node_t) * 2, 0600 | IPC_CREAT | IPC_EXCL);
    usersPID_ID = shmget(SHM_USERS_ARRAY, (SO_USERS_NUM) * sizeof(user), 0600 | IPC_CREAT | IPC_EXCL);
    ledger_ID = shmget(SHM_LEDGER, sizeof(ledger), 0600 | IPC_CREAT | IPC_EXCL);
    if (errno == EEXIST)
    {
        exit(1); /* we could remove them at the start just in case, but I want the error not to be hidden */
    }

    Ledger = (Block_*)shmat(ledger_ID, NULL, 0);
    usersPid = (user *)shmat(usersPID_ID, NULL, 0);
    nodesPid = (node_t *)shmat(nodesPID_ID, NULL, 0);

    /* mark for deallocation so that they are automatically
     * removed once master dies
     * this will set the key to 0x00000000
     */
    shmctl(usersPID_ID, IPC_RMID, NULL);
    shmctl(nodesPID_ID, IPC_RMID, NULL);
    shmctl(par_ID, IPC_RMID, NULL);
    shmctl(ledger_ID, IPC_RMID, NULL);

    shmArray[0] = usersPID_ID;
    shmArray[1] = nodesPID_ID;
    shmArray[2] = par_ID;
    shmArray[3] = ledger_ID;
}


void sems_init()
{

	semUsersPid_Id = semget(SEM_USERS_PIDS_KEY, 1, 0600 | IPC_CREAT | IPC_EXCL);
	semNodesPid_Id = semget(SEM_NODES_PIDS_KEY, 1, 0600 | IPC_CREAT | IPC_EXCL);
	semLedger_Id = semget(SEM_LEDGER_KEY, 1, 0600 | IPC_CREAT | IPC_EXCL);

	switch (errno)
	{
	case EEXIST:
		printf("[MASTER] one or more sems couldn't be created\n");
		exit(1);
	case ENOSPC:
		printf("[MASTER] too many sems in the system");
		exit(1);

	default:
		printf("[MASTER] semUsersPid_id is %d\n", semUsersPid_Id);
		printf("[MASTER] semNodesPid_id is %d\n", semNodesPid_Id);
		printf("[MASTER] semLedger_id is %d\n", semLedger_Id);
		break;
	}
}
void create_arguments(int* IPC_array, char** argv)
{
	char uPid_array[13] = { 0 };
	char nPid_array[13] = { 0 };
	char ledger[13] = { 0 };
	char semUserPid_Id[13] = { 0 };
	char semNodePid_Id[13] = { 0 };
	char semLedger_Id[13] = { 0 };
	printf("creating arguments\n");
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
	/*key_t pidGot = getpid();*/
	key_t pidGot = M_QUEUE_KEY;
	int queue;
	 queue =msgget(M_QUEUE_KEY, IPC_CREAT | 0666);
	 if (queue <= -1 )
	 {
		fprintf(stderr,"[MASTER : ] PROBLEM IN QUEUE \n");
		return PROBLEM; 
	 }
	 
	TRANSACTION_MTYPE;
	/*
	switch (errno)
	{
	case EIDRM:
		printf("[PROCESS %d] queue %d was removed\n", getpid(),pidGot);
		break;
	case EINVAL:
		printf("[PROCESS %d] queue %d invalid value for cmd or msqid\n", getpid(),pidGot);
		printf("Message queue %d removed\n",pidGot);
		break;
	case EPERM:
		printf("[PROCESS %d] queue %d the effective user ID of the calling process "
					 "is not the creator or the owner\n",
				getpid(),pidGot);
				
		break;
		
	}
	*/
printf("[MASTER %d QUEUE PID IS %d  , STATUS --> CREATION SUCCESS] ",pidGot,queue); 
return pidGot; 
}
void Sh_MemMaster(key_t key, size_t size, int shmflg)
{
	/*int m_id; TODO remove this if useless at th end*/
	int ShInit;
	char* shm;
	int *shmdet;
	ShInit = shmget(key, sizeof(SO_REGISTRY_SIZE) * 2, IPC_CREAT | 0666);
	/*ShdMem Define Area*/        /* Raddoppio l'area per evitare saturazioni*/
	shm = shmat(ShInit, NULL, 0); /*Attach ShMem;*/
	shmdet = shmdt(ShInit);       /*Detach ShMem*/
	system("./Users");
}

void Sh_UserPID(key_t key, size_t size, int shmflg)
{
	int Sh_UserPIDDet;
	int* Sh_UserPIDInit;
	/*int id; TODO delete this if is actually useless*/
	char* Sh_UserPIDAttach;
	key = getpid();
	Sh_UserPIDInit = shmget(key, sizeof(SO_USERS_NUM), IPC_CREAT | 0666);
	Sh_UserPIDAttach = shmat(Sh_UserPID, NULL, 0);
	Sh_UserPIDDet = shmdt(Sh_UserPIDInit);
}

void Shared_Memory(key_t key, size_t size, int shmflg)
{
	/*int m_id;*/
	int* shm;
	int ShInit;
	ShInit = shmget(key, sizeof(SO_REGISTRY_SIZE) * 2,IPC_CREAT | 0666); /*ShdMem Define Area*/
	shm = shmat(ShInit, NULL, 0);      /*Attach ShMem;*/
	int shmdt(const void* shmaddr);    /*Detach ShMem*/
}

void make_ipc_array(int *IPC_array)
{
    int shmIDs[SHM_NUM]; /* array containing every shared memory ID */
    int semIDs[SEM_NUM];

    shared_memory_objects_init(shmIDs);
    semIDs[0] = semUsersPid_Id;
    semIDs[1] = semLedger_Id;
    semIDs[2] = semNodesPid_Id;
    /* semaphores_init(semIDs); */
    memcpy(IPC_array, shmIDs, SHM_NUM * sizeof(int));
    memcpy(IPC_array + SHM_NUM, semIDs, SEM_NUM * sizeof(int));
    
}
/* generate the user with fork and lauch ./users with execve*/
void generate_user(int uCounter, char* userArgv[])
{
	pid_t uPid = fork();

	switch (uPid)
	{
	case -1:
		printf("Error in fork for user\n");
		break;
	case 0:
		printf("[PROCESS %d] Forked child %d\n", getpid(), getpid());
		message_queue_id();
    	execve(USER_NAME,userArgv,NULL); 
		break;
	default:
		usersPid[uCounter].usPid = uPid;
		usersPid[uCounter].Us_state = ALIVE;
		usersPid[uCounter].balance = 0;

		return;
	}
}

int generate_node(int nCounter, char* nodeArgv[])
{
	static unsigned int buff = 0;
	pid_t nPid = fork();

	printf("generating node\n");
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
		printf("[MASTER QUEUE ] BEFORE QUEUE FUNCTION \n");
		message_queue_id();
		printf("[MASTER] QUEUE AFTER CALL FUNCTION \n "); 
        execve(NODE_NAME,nodeArgv,NULL);
		break;

	default:
		nodesPid[nCounter].nodPid = nPid;
		nodesPid[nCounter].balance = 0;
		nodesPid[nCounter].Node_state = available;
		break;
	}
	return nPid;
}

/*Ctrl-C handler*/
void signal_handler(int signum)
{
	killpg(0,SIGINT);


	printf("Parent: signal recieved %d\n", signum);
	semctl(semUsersPid_Id, 1, IPC_RMID);
	semctl(semNodesPid_Id, 1, IPC_RMID);
	semctl(semLedger_Id, 1, IPC_RMID);
	/*  msgctl(mQueue, IPC_RMID, MessageQ); */

	printf("User pressed CTRL-C\n");
	exit(0);
}

int main(int argc,char *argv[])
{
	unsigned int i;
	unsigned int uCounter;
	unsigned int nCounter;
	int ipcObj[IPC_NUM];
	char* argvCreator[8];
	struct sigaction sa;
	struct sembuf sops;

    
	for (i = 0; i < 8; i++)
		argvCreator[i] = malloc(3 * sizeof(int) + 1);

	printf("[MASTER] ---> main");
	tmpLedger = ledger;
	
	printf("Master PID: %d\n", getpid());
	make_ipc_array(ipcObj);

	create_arguments(ipcObj,argvCreator);
    if(argc < 1)
    {
        printf("Missing arguments, maybe you need to do <source setting.conf>\n");
        exit(1);
    }
	printf("before sems_init\n"); /*TODO: remove,debug only*/
	sems_init();
	printf("after sems_init\n"); /*TODO: remove,debug only*/

	/* set all sigaction's byte to zero
	 sa.handler to handle signal_handler(),then the handler is set to handle
	 SIGINT signals
	*/

	bzero(&sa, sizeof(sa));
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGALRM, &sa, NULL);
	printf("before sigaction\n"); /*TODO: remove,debug only*/
	/*signal(SIGUSR1, signal_handler);*/
	mQueue = message_queue_id();
	printf("before for loop\n"); /*TODO: remove,debug only*/
	argvCreator[0]=NODE_NAME;
	for (nCounter = 0; nCounter <= SO_NODES_NUM;nCounter++) /*TODO: seg fault here imo, need to solve, FIXME: just for debug purpose */
	{
	
		if (nCounter > SO_NODES_NUM)
		{
			printf("[MASTER USER FUNCTION ] | GENRATION USERS COMPLETE  \n");
			break;
		}
		
		int sigum;
		printf("nCounter: %d\n", nCounter);
		printf("in for loop\n"); /*TODO: remove,debug only*/
		/*nodesPid[nCounter].balance = 0; /*TODO seg fault here
		nodesPid[nCounter].Node_state = available; */
		generate_node(nCounter, argvCreator);
		sleep(5);
		/*signal_handler(sigum);*/
		/*signal_handler(sigum);*/
	}
	argvCreator[0]=USER_NAME;
	for (uCounter = 0; uCounter <= SO_USERS_NUM; uCounter++)
	{	
		int sigum;
		/*usersPid[uCounter].Us_state = ALIVE;
		usersPid[uCounter].balance = 0;*/
		generate_user(uCounter, argvCreator);
		sleep(5);
		if (uCounter > SO_USERS_NUM )
		{
			printf("[MASTER NODE ] NODE GENERATION COMPLETE \n"); 
			break;
		}
		
	}

	alarm(SO_SIM_SEC);

	switch (fork())
	{
	case -1:
		fprintf(stderr, "[MASTER]] fork error\n");
		exit(EXIT_FAILURE);
		break;
	case 0:
	{
		unsigned int i;
		unsigned int tmpPid;

		/*friend_msg newNode;*/ 
		Message transHop;

		/*bzero(&newNode , sizeof(newNode));*/
		bzero(&transHop, sizeof(transHop));
		signal(SIGINT, SIG_DFL);

		while (1)
		{
			/*receive_message(mQueue, &newNode, sizeof(Message), TRANSACTION_MTYPE, 0);*/ 
			nodesPid[nCounter].Node_state = ALIVE;
			nodesPid[nCounter].balance = 0;
			tmpPid = generate_node(argvCreator,-1);
		}
	}
	default:

		break;
	}
	return 0;
 }

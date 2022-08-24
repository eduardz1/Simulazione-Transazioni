#include "include/Master.h"
#include "include/Common.h"
#include <stdlib.h>


#define USER_NAME "./Users"
#define NODE_NAME "./Nodes"

user *usersPid;
node *nodesPid;
Block_ ledger[SO_REGISTRY_SIZE];
Block_ *tmpLedger;
int mQueue;


int receive_message(int queueID,void *msg,int size,int mtype,int flag){
  if(msgrcv(queueID,msg,size,mtype,flag)==0){
    return 0; /*SUCCESS*/
  }
  switch (errno)
  {
    case EACCES:
      fprintf(stderr,"[MSG SEND %d] no write permission on queue\n", getpid());
      break;
    case EAGAIN:
      fprintf(stderr,"[MSG SEND %d] couldn't write on queue\n", getpid());
      break;
    case EFAULT:
      fprintf(stderr,"[MSG SEND %d] address pointed by msgp inaccessible\n", getpid());
      break;
    case EIDRM:
      fprintf(stderr,"[MSG SEND %d] message queue removed\n", getpid());
      break;
    case EINTR:
      fprintf(stderr,"[MSG SEND %d] signal caught when waiting for queue to free\n", getpid());
      break;
    case EINVAL:
      fprintf(stderr,"[MSG SEND %d] invalid  msqid  value,  or non positive mtype value, or invalid msgs value\n", getpid());
      break;
    case ENOMEM:
      fprintf(stderr,"[MSG SEND %d] system out of memory\n", getpid()); /* hoping this never happens */
      break;
    default:
      return 0;
      break;
  }
  return -1;
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
	pid_t uPid= fork();

	switch(uPid){
		case -1:
		 	printf("Error in fork for user\n");
		break;
		case 0:
		 	printf("[PROCESS %d] Forked child %d\n", getpid(), getpid());
		 	system(USER_NAME);
		 break;
		default:
			usersPid[uCounter].usPid=uPid;
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
	int i;
	unsigned int uCounter;
	unsigned int nCounter;

	struct sigaction sa;

	tmpLedger = ledger;

	/*signal handler function 
	* set all sigaction's byte to zero
	* sa.handler to handle signal_handler(),then the handler is set to handle SIGINT signals
	*/

	bzero(&sa, sizeof(sa));
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGALRM, &sa, NULL);
	mQueue = message_queue_id();

	for (nCounter = 0; nCounter < SO_NODES_NUM; nCounter++) /*TODO: seg fault here imo, need to solve */
	{
		nodesPid[nCounter].Node_state = ALIVE;
		nodesPid[nCounter].balance = 0;
		generate_user(nCounter);
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
		bzero(&newNode,sizeof(newNode)); /*set memory region to zero */
		bzero(&transHop,sizeof(transHop));
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

#include "include/Users.h"

Message *tns;
node_t *sendingTransaction;
user *usersPid;
node_t *nodesPid;
int queueID;
pid_t myPid;
unsigned int currBalance;
struct node *outGoingTransactions = NULL;
Block_ *ledger;

int semUsersPids_id;
int semLedger_id;

/*function to handle transaction pool easily(linked list util) */

int get_reward(unsigned int amount,int reward)
{
  return ceil(reward * amount / 100);
}

unsigned int get_rand(unsigned int min,unsigned int max)
{
  return rand() % (max - min + 1) + min;
}

/* returns a random PID of a non-dead user from usersPID[] */
pid_t get_random_userPID()
{
  int index;
  pid_t val = 0;

  do
  {
    index = get_rand(0, SO_USERS_NUM*2 - 1);
    if (usersPid[index].Us_state != DEAD && usersPid[index].usPid != myPid)
      val = usersPid[index].usPid;
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
    index = get_rand(0, SO_NODES_NUM * 2 - 1);
    if (nodesPid[index].Node_state == available && nodesPid[index].nodPid != 0)
      val = nodesPid[index].nodPid;
  } while (!val);

  return val;
}

struct node *new_node(transaction t)
{
 struct node *newNode = malloc(sizeof(struct node));
  if (newNode == NULL)
  {
    printf("Error allocating memory for new node\n");
  }
   newNode->trans=t;  /*TODO why is this not working? */
  newNode->next = NULL;
  return newNode;
}

/*set transaction at the end of the linked list*/
void push(struct node *head, transaction t)
{
  struct node *curr = head;
  while (curr->next != NULL)
  {
    curr = curr->next;
  }
  curr->next = new_node(t);
}

int compare_transaction(transaction *t1, transaction *t2)
{
  if (t1->Money == t2->Money && t1->Receiver == t2->Receiver && t1->time.tv_nsec == t2->time.tv_sec && t1->time.tv_nsec && t2->time.tv_nsec)
  {
    return 1;
  }
  return 0;
}

/* find and removes a message from pool if present */
void find_and_remove(struct node **head, transaction *search)
{
  struct node *curr = *head;
  struct node *prev = NULL;

  if (*head == NULL)
  {
    return;
  }
  while (!compare_transaction(&curr->trans, search))
  {
    if (curr->next == NULL)
    {
      return; /* transaction not found */
    }
    else
    {
      prev = curr;
      curr = curr->next;
    }
  }
  if (curr == *head)
  {
    free(*head);
    *head = (*head)->next;
  }
  else
  {

    prev->next = curr->next;
    free(curr);
  }
}



/* try to attach to queue of nodePid key until it succed */
void queue_to_pid(pid_t nodePid)
{
  do
  {
    errno = 0;
    queueID = msgget(nodePid, 0);
  } while (errno == ENOENT);
}

int get_pid_userIndex(int searchPid)
{
  unsigned int i;
  printf("\n");
  printf("get_pid_userIndex function\n");
  printf("searchPid %d\n", searchPid);
  for (i = 0; i < SO_USERS_NUM - 1; i++)
  {
    printf("for loop %d\n", i);         /*debug*/
    if (usersPid[i].usPid == searchPid) /*FIXME: solve seg fault*/
      return i;
    printf("here segfault\n"); /* debug */
    printf("found user %d\n", i);
    return i;
  }
  return PROBLEM; /*error*/
}

/* update status of user , NB--> 0 alive, 1 broke, 2 dead*/
void update_status(int setStatus)
{
  int i = get_pid_userIndex(myPid);

  printf("[%d] updating status to %d\n", myPid, setStatus);
  if (i == -1)
  {
    printf("user failed to find");
  }
  resource_set(semUsersPids_id, i);
  usersPid[i].Us_state = setStatus; /* maybe a semaphore is needed to protect this section  debug*/

  if (setStatus == 2)
  {
    printf("[USERS] dead increased");
  }
  resource_release(semUsersPids_id, i);
}

void start_transaction(pid_t userPid, int money, int reward)
{
  struct timespec exTime = {0};
  clock_gettime(CLOCK_REALTIME, &exTime);

  tns->m_type = TRANSACTION_MTYPE;
  tns->Message_Transaction.uTrans.Sender = myPid;
  tns->Message_Transaction.uTrans.Receiver = userPid;
  tns->Message_Transaction.uTrans.Money = money;
  tns->Message_Transaction.uTrans.Reward = reward;
  tns->Message_Transaction.uTrans.MoneyStatusTrans = pending;
  tns->Message_Transaction.uTrans.time = exTime;
  tns->Message_Transaction.hops = SO_HOPS;
}

int send_transaction()
{
  transaction sent = {0};
  if (msgsnd(queueID, &tns, sizeof(tns), IPC_NOWAIT) == 0)
  {
    printf("[USER %d] sent a transaction of %d UC to [USER %d] via queue %d\n", myPid, tns->Message_Transaction.uTrans.Money, tns->Message_Transaction.uTrans.Receiver, queueID);
    currBalance -= (tns->Message_Transaction.uTrans.Money + tns->Message_Transaction.uTrans.Reward);
    sent = tns->Message_Transaction.uTrans;
    if (outGoingTransactions == NULL)
    {
      outGoingTransactions = new_node(sent);
    }
    else
    {
      push(outGoingTransactions, sent);
    }
    return 0;
  }
  return -1; /*error*/
}
void attach_ipc_objects(char **argv)
{
	
	
	usersPid = shmat(USERS_PID_ARGV, NULL, 0);
	nodesPid = shmat(NODES_PID_ARGV, NULL, 0);
	ledger = shmat(LEDGER_ARGV, NULL, 0);
	semUsersPids_id = SEM_USERS_PIDS_ARGV;
	semLedger_id = SEM_LEDGER_ARGV;
}

void Sh_MemUser(key_t key, size_t size, int shmflg)
{
  int Mem_id;
  int Sh_MemInit = shmget(key, sizeof(SO_USERS_NUM), IPC_CREAT | 0666); /*define area*/
  char *shmAttach = shmat(Sh_MemInit, NULL, 0);                         /*Attach Area*/
  int *ShDet = shmdt(Sh_MemInit);                                        /*Detach Area*/
}

void update_balance(unsigned int tmpBalance)
{
  int i = get_pid_userIndex(myPid);
  printf("[%d] updating balance to %d\n", myPid, tmpBalance);
  resource_set(semUsersPids_id, i);
  currBalance = tmpBalance;
  usersPid[i].balance = currBalance;
  resource_release(semUsersPids_id, i);
}

/*saves user balance when the program is interrupted in tmpBalance*/
void current_balance()
{
  
  int i;
  int j;
  long accumulate = 0;
  long flag = 1;
  unsigned int tmpBalance = SO_BUDGET_INIT;
  struct node  *tmp; 
  Block_ tmpLedger[SO_REGISTRY_SIZE];
  resource_set(semLedger_id, 1);
  memcpy(&tmpLedger, ledger, sizeof(tmpLedger));
  resource_release(semLedger_id, 1);

  printf("current balance function\n");
  printf("[USER %d] current balance is %d\n", myPid, currBalance);
  for (i = 0; i < SO_REGISTRY_SIZE && flag != 0; i++)
  {
    /*can't have time =0, otherwise it means the block isn't initialized*/
    flag = (tmpLedger[i].t_list->time.tv_nsec) + (tmpLedger[i].t_list->time.tv_sec);
    printf("flag is %ld\n", flag); /*debug*/
    /*if transaction is out-going remove Money+Reward else add to receiver Money */
    for (j = 1; j < SO_BLOCK_SIZE && flag != 0; j++)
    {
      printf("Sender is %d\n", tmpLedger[i].t_list->Sender); /*FIXME: remove, just for debug*/
      if (tmpLedger[i].t_list[j].Sender == myPid)
      {
        find_and_remove(&outGoingTransactions, &tmpLedger[i].t_list[j]);
        accumulate -= (tmpLedger[i].t_list[j].Money + tmpLedger[i].t_list[j].Reward);
      }
      else if (tmpLedger[i].t_list[j].Receiver == myPid)
      {
        accumulate += tmpLedger[i].t_list[j].Money;
      }
    }
  }
  tmp=outGoingTransactions;
  while (tmp != NULL)
  {
    accumulate -= (tmp->trans.Money + tmp->trans.Reward);
    tmp = tmp->next;
    
  } 

/*  resource_release(semUsersPids_id, i);*/
  printf("accumulate %ld\n", accumulate);
  if (accumulate * (-1) > tmpBalance)
{
  {
    fprintf(stderr, "*** [USER %d] errror in calculating balance, overflow ***\n", myPid);
    update_status(2);
    killpg(0, SIGINT);
  }
  printf("balance is %d\n", tmpBalance);
}
tmpBalance+=accumulate;

if (errno == ERANGE)
{ /* not working as intended */
	{
		fprintf(stderr, "[USER %d] went out of bound, punishment for being that rich is death\n", myPid);
		update_status(2);
		kill(myPid, SIGINT);
	}

	update_balance(tmpBalance);
}
}
void user_transaction_handle(int signum)
{
  int retry = SO_RETRY;

  write(1, "::USER:: SIGUSR1 received\n", 27);

  current_balance();
  if (currBalance >= 2)
  {
    pid_t userPid = get_random_userPID();
    pid_t nodePid = get_random_nodePID();

    unsigned int amount = get_rand(1, currBalance);
    unsigned int reward = get_rand(1, amount);
    amount -= reward;
    update_status(0);

    start_transaction(userPid,amount, reward);

    if (send_transaction() == 0)
    {
      retry = SO_RETRY;
    }
    else
    {
      retry--;    }
    if (retry == 0)
    {
      update_status(2);
      kill(myPid, SIGINT);
    }
  }
  else
  {
    write(1, "::USER:: sorry balance too low :(\n", 32);
  }

  update_balance(currBalance);
}


void user_signal_handler(int signum){
	struct node *tmp;
	int i = get_pid_userIndex(myPid);
	current_balance();

	if(usersPid[get_pid_userIndex(myPid)].Us_state!=DEAD)
	{
		if(usersPid[i].balance >=2)
			update_status(0);
		else
			update_status(1);
	}
	tmp = outGoingTransactions;
	resource_set(semUsersPids_id,1);
	while(tmp != NULL){
		usersPid[i].balance += (tmp->trans.Money + tmp->trans.Reward);
		tmp=tmp->next;
	}
	resource_release(semUsersPids_id,1);
	free(tmp);
	killpg(0,SIGINT);
	exit(0);
}

void signal_handler_user_init(struct sigaction *saUSR1, struct sigaction *saINT)
{
	saUSR1->sa_handler = user_transaction_handle;
	saINT->sa_handler = user_signal_handler;
	sigaction(SIGUSR1,saUSR1,NULL);
	sigaction(SIGINT,saINT,NULL);
}

int main(int argc, char *argv[])
{
  unsigned int amount, reward, retry;
  pid_t usPid, ndPid;
 struct sigaction saUSR1;
	struct sigaction saInt_u;

  currBalance = SO_BUDGET_INIT;
	bzero(&saUSR1,sizeof(saUSR1));
  bzero(&saInt_u,sizeof(saInt_u));
  printf("[USER]--->main\n");
  
  myPid = getpid();
  

  
  
	if (argc==0){
		perror("[USER] no arguments passed");
		exit(EXIT_FAILURE);
	}
  srand(myPid); /*initialize rand function, so we have same pattern for each user*/

  attach_ipc_objects(argv);
  signal_handler_user_init(&saUSR1, &saInt_u);
  tns->m_type=TRANSACTION_MTYPE;
  currBalance=SO_BUDGET_INIT;
  retry = SO_RETRY;
  while (1)
  {
    current_balance();

    if (currBalance >= 2)
    {
      update_status(0);

      usPid = get_random_userPID();
      ndPid = get_random_nodePID();

      amount = get_rand(2, currBalance);
      reward = get_reward(amount, SO_REWARD);
      amount -= reward;

      queue_to_pid(ndPid);
      start_transaction(usPid,amount, reward);

      if (send_transaction() == 0)
      {
        retry = SO_RETRY;
      }
      else
      {
        retry--;
      }

      if (retry == 0)
      {
        update_status(2);
        printf("[USER %d] max retry reached, life has no meaning any more\n", myPid);
        kill(myPid, SIGINT);
      }

      else
      {
        update_status(1);
      }
    }
  }
 }
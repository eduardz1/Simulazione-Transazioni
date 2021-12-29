#include "include/master.h"

pid_t spawn_user(userArgv, shmID)
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        /* error handling goes here */
        break;

    case 0: /* Child case */
        execve("user", userArgv, NULL);
        break;

    default:
        return myPID;
    }
}

pid_t spawn_node(nodeArgv, shmID)
{
    pid_t myPID = fork();
    switch (myPID)
    {
    case -1: /* Error case */
        /* error handling goes here */
        break;

    case 0: /* Child case */
        execve("node", nodeArgv, NULL);
        break;

    default:
        return myPID;
    }
}

void interrupt_handle(signum)
{
    shmctl(shmID, IPC_RMID, NULL);
}

int main(int argc, char *argv[]) /* 
								  *  [1] unsigned int SO_USER_NUM
								  *  [2] unsigned int SO_NODES_NUM
								  *  [3] unsigned int SO_NUM_FRIENDS
								  *  [4] unsigned int SO_SIM_SEC
                                  *  -------------------------------
                                  *  [5] unsigned int SO_BUDGET_INIT
								  *  [6] unsigned int SO_REWARD
								  *  [7] unsigned int SO_RETRY
								  *  [8] unsigned long SO_MIN_TRANS_GEN_NSEC
								  *  [9] unsigned long SO_MAX_TRANS_GEN_NSEC
								  *  -------------------------------
								  *  [10]
								  *  [11]
								  */
{
    int u, n; /* u counter for user processes, n counter for node processes */
    int shmID; /* ID of "ledger" shared memory segment */
    unsigned int userNum, nodesNum, friendsNum, simTime;
    pid_t myPID = getpid();
    pid_t myPPID;

    char *userArgv[5];
    char *nodeArgv[2];

    struct ledger ledger;
    struct sigaction sa; /* we need to define an handler for CTRL-C command that closes any IPC object */

    /* -- CL PARAMETERS INITIALIZATION -- */
    userNum = atoi(argv[1]);
    nodesNum = atoi(argv[2]);
    friendsNum = atoi(argv[3]);
    simTime = atoi(argv[4]);

    /* -- PID ARRAYS -- */
    pid_t arrayUserPID[userNum];
    pid_t arrayNodesPID[nodesNum];

    /* -- USER CL PARAMETERS -- */
    userArgv[0] = argv[5]; /* SO_BUDGET_INIT */
    userArgv[1] = argv[6]; /* SO_REWARD */
    userArgv[2] = argv[7]; /* SO_RETRY */
    userArgv[3] = argv[8]; /* SO_MIN_TRANS_GEN_NSEC */
    userArgv[4] = argv[9]; /* SO_MAX_TRANS_GEN_NSEC */

    /* -- NODE CL PARAMETERS -- */
    nodeArgv[0] = argv[10];
    nodeArgv[1] = argv[11];

    /* -- LEDGER INITIALIZATION -- (still needs the code to initialize it)
     * save the ID of our new (IPC_PRIVATE) shared memory segment of size -ledger- 
     * smctl will deallocate the shared memory segment only when every process detaches it
     * tells OS that ledger of type ledger is our shared memory of shmID
     */
    shmID = shmget(IPC_PRIVATE, sizeof(*ledger), 0600);
    shmctl(shmID, IPC_RMID, NULL);
    ledger = shmat(shmID, NULL, 0);

    /* -- SIGNAL HANDLER --
	 * first set all bytes of sigation to 0
	 * then initialize sa.handler to a pointer to the function interrupt_handle
	 * then set the handler to handle SIGINT signals ((struct sigaction *oldact) = NULL) 
	 */
    bzero(&sa, sizeof(sa));
    sa.sa_handler = interrupt_handle;
    sigaction(SIGINT, &sa, NULL);
    

    for (u; u < userNum; u++)
    {
        arrayUserPID[u] = spawn_user(userArgv, shmID);
    }

    for (n; n < nodesNum; n++)
    {
        arrayNodesPID[n] = spawn_node(nodeArgv, shmID);
    }

    wait(simTime);
    kill(mypid(), SIGINT); /* our sigint handler needs to do quite a lot of things to print the wall of test below */

    print_user_nodes_table(myPID, arrayUserPID, arrayNodesPID, userNum, nodesNum);
    print_kill_signal(); /* need to define, prints reason of termination (simTime elapsed/ledger full/every process terminated) */
    print_user_balance(); /* need to define, prints balance of every user */
    print_node_balance(); /* need to define, prints balance of every node */
    print_num_aborted(); /* need to define, prints num of processes aborted */
    print_num_blocks(); /* need to define, prints num of blocks saved in the ledger */
    print_transactions_still_in_pool(); /* need to define, prints num of transactions still in the pool of each node */

    return 0;
}
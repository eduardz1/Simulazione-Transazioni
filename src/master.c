#include "include/master.h"

int main(int argc, char *argv[]) /* 
								  * (unsigned int SO_USER_NUM, 
								  *  unsigned int SO_NODES_NUM, 
								  *  unsigned int SO_NUM_FRIENDS, 
								  *  unsigned int SO_SIM_SEC,
                                  *  -------------------------------
                                  *  unsigned int SO_BUDGET_INIT, 
								  *  unsigned int SO_REWARD, 
								  *  unsigned int SO_RETRY, 
								  *  unsigned long SO_MIN_TRANS_GEN_NSEC, 
								  *  unsigned long SO_MAX_TRANS_GEN_NSEC)
								  */
{
    int u, n; /* u counter for user processes, n counter for node processes */
    unsigned int userNum, nodesNum, friendsNum, simTime;
    pid_t myPID = getpid();
    pid_t myPPID;

    char *userArgv[5];
    char *nodeArgv[7]; /* 7 is a dummy value */

    struct node *head = NULL;
    struct node *current = NULL;
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
        arrayUserPID[u] = spawn_user(userArgv);
    }

    for (n; n < nodesNum; n++)
    {
        arrayNodesPID[n] = spawn_node(nodeArgv);
    }

    print_user_nodes_table(myPID, arrayUserPID, arrayNodesPID, userNum, nodesNum);
}

pid_t spawn_user(char** userArgv)
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

pid_t spawn_node(char** nodeArgv)
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

void interrupt_handle(int signum)
{
    /*
     * remove any IPC object we created then terminate the process
     */
}

int printPid()
{
    int i = 0;
    int k = 0;
    for (int i; i < SO_NODES_NUM; i++)
    {
        printf("generated SO_NODES pid\n %d\n", getpid());
    }
    for (int k; k < SO_USERS_NUM; k++)
    {
        printf("generated SO_USERS pid\n %d\n", getpid());
    }
}
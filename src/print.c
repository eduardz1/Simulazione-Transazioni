#include "include/print.h"

/* #define HYPHEN "--------" */

void print_time_to_die()
{
    printf("\n***********************\n|| The time has come ||\n***********************\n\n");
}

void print_user_nodes_table(pid_t mainPID, user *userPID, node *nodePID, struct parameters *par)
{
    int userNum = par->SO_USER_NUM;
    int nodesNum = par->SO_NODES_NUM;

    printf("\n ------- Master Process PID is %d ----------\n", mainPID);
    printf("|                                             |\n");
    printf(" - Type ------- PID --------- Status ---------\n");
    printf(" ---------------------------------------------\n");
    while (userNum--)
    {
    printf("|  User         %d            %d             |\n", userPID[userNum].pid, userPID[userNum].status);
    }
    printf(" ---------------------------------------------\n");
    while (nodesNum--)
    {
    printf("|  Node         %d            %d             |\n", nodePID[nodesNum].pid, nodePID[nodesNum].status);
    }
    printf(" ---------------------------------------------\n");
}

void print_kill_signal();
void print_user_balance();
void print_node_balance();
void print_num_aborted();
void print_num_blocks();
void print_transactions_still_in_pool();

void final_print(pid_t masterPID, user *usersPID, node *nodesPID, struct parameters *par)
{
    print_user_nodes_table(masterPID, usersPID, nodesPID, par);
    /*print_kill_signal();
    print_user_balance();
    print_node_balance();
    print_num_aborted();
    print_num_blocks();
    print_transactions_still_in_pool();*/
}

void print_parameters(struct parameters *par){
printf("--------------------------------------------\n----------- Configuration value ------------\n");
printf("SO_USER_NUM->%u\n", par->SO_USER_NUM);
printf("SO_NODES_NUM->%u\n", par->SO_NODES_NUM);
printf("SO_BUDGET_INIT->%u\n", par->SO_BUDGET_INIT);
printf("SO_REWARD->%u\n", par->SO_REWARD);
printf("SO_MIN_TRANS_GEN_NSEC->%lu\n", par->SO_MIN_TRANS_GEN_NSEC);
printf("SO_MAX_TRANS_GEN_NSEC->%lu\n", par->SO_MAX_TRANS_GEN_NSEC);
printf("SO_RETRY->%u\n", par->SO_RETRY);
printf("SO_TP_SIZE->%u\n", par->SO_TP_SIZE);
printf("SO_MIN_TRANS_PROC_NSEC->%lu\n", par->SO_MIN_TRANS_PROC_NSEC);
printf("SO_MAX_TRANS_PROC_NSEC->%lu\n", par->SO_MAX_TRANS_PROC_NSEC);
printf("SO_SIM_SEC->%u\n", par->SO_SIM_SEC);
printf("SO_FRIENDS_NUM->%u\n", par->SO_FRIENDS_NUM);
printf("SO_HOPS->%u\n", par->SO_HOPS);
printf("--------------------------------------------\n");
}

/*void print_kill_signal(mainPID, userPid /* other process *)
{
    printf("-----PROCESS PID NUM %d KILL----", mainPID);
}
void print_user_balance(int balance)
{
    printf("-----CURRENT BALANCE IS:%d-----", balance);
}

void print_node_balance(int balamce)
{
    printf("----CURRENT NODE BALANCE IS:%d", balance);
}
void print_num_aborted()
{
    printf("----NUMB OF ABORT PROCESS IS :%d");
}
void print_num_blocks()
{
    printf("---TOTAL BLOCK:%d");
}
void print_transactions_still_in_pool()
{
    printf("----TOTAL TRANSACTION STILL IN POLL:%d----");
}*/
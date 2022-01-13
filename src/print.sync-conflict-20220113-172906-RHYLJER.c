#include "include/print.h"

/* #define HYPHEN "--------" */

void print_time_to_die()
{
    printf("\n***********************\n|| The time has come ||\n***********************\n\n");
}

void print_user_nodes_table(pid_t mainPID, user *userPID, node *nodePID, int userNum, int nodesNum)
{
    printf(" ------- Master Process PID is %d ----------\n", mainPID);
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
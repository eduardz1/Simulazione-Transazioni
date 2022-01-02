#include "include/print.h"
/* #define HYPHEN "--------" */

/* idk why it's broken */
void print_user_nodes_table(mainPID, userPID, nodePID, userNum, nodesNum)
{
    printf("-------- Master Process PID is %d --------\n", mainPID);
    printf("------------------------------------------\n");
    while (userNum--)
    {
        printf("User %d\n", userPID[&userNum]);
    }
    printf("------------------------------------------\n");
    while (nodesNum--)
    {
        printf("Node %d\n", nodePID[&nodesNum]);
    }
}

void print_kill_signal(); 
void print_user_balance();
void print_node_balance();
void print_num_aborted(); 
void print_num_blocks(); 
void print_transactions_still_in_pool(); 
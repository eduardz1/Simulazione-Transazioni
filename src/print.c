#include "include/print.h"
// #define HYPHEN "--------"

void print_user_nodes_table(mainPID, userPID, nodePID, userNum, nodesNum)
{
    printf("-------- Master Process PID is %d --------\n", mainPID);
    printf("------------------------------------------\n");
    while (userNum--)
    {
        printf("User %d\n", userPID[userNum]);
    }
    printf("------------------------------------------\n");
    while (nodesNum--)
    {
        printf("Node %d\n", nodePID[nodesNum]);
    }
}
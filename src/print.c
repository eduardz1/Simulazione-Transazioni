
#include "include/common.h"
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
    print_node_balance();*
    print_num_aborted();
    /*
    print_num_blocks();
    print_transactions_still_in_pool();*/
}

void print_parameters(struct parameters *par)
{
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
} */
/*void print_num_aborted()
{
    printf("\n-- Num of aborted users: %d\n", usersPrematurelyDead);
}
/*
void print_num_blocks()
{
    printf("---TOTAL BLOCK:%d");
}
void print_transactions_still_in_pool()
{
    printf("----TOTAL TRANSACTION STILL IN POLL:%d----");
}*/

void print_transaction(FILE *fp, transaction *t)
{
    char tmp[10];
    switch (t->status)
    {
    case pending:
        strcpy(tmp, "pending");
        break;
    case aborted:
        strcpy(tmp, "aborted");
        break;
    case confirmed:
        strcpy(tmp, "confirmed");
        break;
    case processing:
        strcpy(tmp, "confirmed");
        break;
    }

    fprintf(fp, " -------------------------- \n");
    formatted_timestamp(fp);
    fprintf(fp, "    %s", tmp);
    fprintf(fp, "|  %d --> %d\n", t->sender, t->receiver);
    fprintf(fp, "|  Amount:    %d\n", t->amount);
    fprintf(fp, "|  Reward:    %d\n", t->reward);
    fprintf(fp, "|  Reward:    %d\n", t->reward);
    fprintf(fp, " -------------------------- \n");
}

void print_block(FILE *fp, block *b)
{
    int i;
    block *curr;

    for (curr = b; curr != NULL; curr = (block*)curr->next)
    {
        fprintf(fp, "= %.3d =======================\n", b->blockIndex);
        for (i = 0; i < SO_BLOCK_SIZE; i++)
        {
            print_transaction(fp, &(b->transList[i]));
        }
        fprintf(fp, "============================\n");
    }
}

void print_ledger(ledger *l)
{
    FILE *fp = fopen("ledger.txt", "w");
    if (fp == NULL)
    {
        printf(":print: coudln't open file pointer ledger.txt\n");
    }

    fprintf(fp, "Registry Real Size is %d blocks\n", l->registryCurrSize);
    print_block(fp, l->head);
    fclose(fp);
}

/* print without /n */
void formatted_timestamp(FILE *fp)
{
    printf("Hey");
    /*clock_t tic = clock();
    clock_t start = clock();
    clock_t stop = clock();

    time_t rawtime;
    time_t now;
    struct tm *info;
    struct tm *today;
    double elapsed;
    char buf[128];

    time(&now);
    today = localtime(&now);
    strftime(buf, 128, "%Y/%m/%d", today);
    printf("%s\n", buf);

    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC; /* time ./a.out*/
}
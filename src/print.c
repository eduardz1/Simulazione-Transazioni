
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
    int statusNum = 0;
    char statusStr[] = "available";

    printf("\n ------- Master Process PID is %d ----------\n", mainPID);
    printf("|                                             |\n");
    printf(" - Type ------- PID --------- Status ---------\n");
    printf(" ---------------------------------------------\n");
    while (userNum--)
    {
        statusNum = userPID[userNum].status;
        TEST_ERROR
        switch (statusNum)
        {
        case 0:
            strcpy(statusStr, "alive    ");
            break;
        case 1:
            strcpy(statusStr, "broke    ");
            break;
        case 2:
            strcpy(statusStr, "dead     ");
            break;
        }
        printf("|  User         %d            %s \n", userPID[userNum].pid, statusStr);
    }
    printf(" ---------------------------------------------\n");
    while (nodesNum--)
    {
        statusNum = nodePID[nodesNum].status;
        TEST_ERROR
        switch (statusNum)
        {
        case 0:
            strcpy(statusStr, "available");
            break;
        case 1:
            strcpy(statusStr, "full     ");
            break;
        }
        printf("|  Node         %d            %s\n", nodePID[nodesNum].pid, statusStr);
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

void print_transaction(transaction *t)
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

    printf(" -------------------------- \n");
    /*formattimestamp(fp);*/
    printf("    %s\n", tmp);
    printf("|  %d --> %d\n", t->sender, t->receiver);
    printf("|  Amount:    %d\n", t->amount);
    printf("|  Reward:    %d\n", t->reward);
    printf(" -------------------------- \n");
}

void print_block(block *b)
{
    int i;
    transaction printable;
    printf("[BLOCK %d] =================\n", b->blockIndex);
    for (i = 0; i < SO_BLOCK_SIZE; i++)
    {
        printable = b->transList[i];
        print_transaction(&printable);
    }
    printf("============================\n");
}

void print_ledger(block *l)
{
    FILE *fp = fopen("ledger.txt", "w");
    int i = 0;
    int flag = l[i].transList[0].sender;
    TRACE(("FLAG %lu\n", flag))

    printf("~{LEDGER}~\n\n");
    for (i; flag != 0; i++)
    {
        print_block(&l[i]);

        flag = l[i].transList[0].timestamp.tv_nsec;
    }
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

void print_transaction_pool(pool *transPool)
{
    pool *tmp = transPool;
    int i = 0;
    pid_t pidCaller = getpid();
    transaction printable;

    printf("[NODE %d] printing transaction pool:\n", pidCaller);
    while (tmp->head != NULL)
    {
        printf("[%d]", i);
        printable = tmp->head->transactionMessage.userTrans;
        print_transaction(&printable);
        tmp->head = tmp->head->transactionMessage.next;
        i++;
    }
}
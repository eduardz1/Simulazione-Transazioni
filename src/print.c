
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
    int nodesNum = 0;
    int statusNum = 0;
    char statusStr[] = "available";

    printf(" -------------------------------------------------\n|          Master Process PID is %d\n", mainPID);
    printf("|                                                 \n");
    printf(" - Type ----- PID ----- Status ----- Balance -----\n");
    printf(" -------------------------------------------------\n");
    while (userNum--)
    {
        statusNum = userPID[userNum].status;
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

        /* we should place it in a buffer so that they print a fixed length */
        printf("|  User      %7d    %s    %u\n", userPID[userNum].pid, statusStr, userPID[userNum].balance);
    }
    printf(" -------------------------------------------------\n");
    while (nodesNum++ < par->SO_NODES_NUM * 2 && nodePID[nodesNum].pid != 0)
    {
        statusNum = nodePID[nodesNum].status;
        switch (statusNum)
        {
        case 0:
            strcpy(statusStr, "available");
            break;
        case 1:
            strcpy(statusStr, "full     ");
            break;
        }
        printf("|  Node      %7d    %s    %10lu\n", nodePID[nodesNum].pid, statusStr, nodePID[nodesNum].balance);
    }
    printf(" -------------------------------------------------\n");
}

void print_num_aborted();
void print_num_blocks(block *l)
{
    int i = 0;
    int blockIndex = 0;

    if (l[i].transList[0].timestamp.tv_nsec + l[i].transList->timestamp.tv_sec == 0)
        return;

    for (i = 0; i < SO_REGISTRY_SIZE && blockIndex == 0; i++)
    {
        if (l[i].transList[0].timestamp.tv_nsec + l[i].transList->timestamp.tv_sec == 0)
            blockIndex = l[i - 1].blockIndex;
    }
    printf("NUM BLOCKS: %d\n", blockIndex);
}
void print_transactions_still_in_pool();

void print_kill_signal()
{
    /* switch ()
     {
     case 0: /* too much time
        if(SO_SIM_SEC>=time(30)){
          printf("TOO MUCH TIME SIMULATION ENDED",);
          exit(EXIT_FAILURE);
        }
         break;
       case 1: /* Registry space is full
        if (SO_REGISTRY_SIZE>100) {
            printf("REGISTRY IS FULL");
            exit(EXIT_FAILURE);
        }
         break;

     case 2 :
 */
}

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

void print_transaction(transaction *t, FILE *fp)
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
    /*formafp, ttimestamp(fp);*/
    fprintf(fp, "|  %s\n", tmp);
    fprintf(fp, "|  %d --> %d\n", t->sender, t->receiver);
    fprintf(fp, "|  Amount:    %d\n", t->amount);
    fprintf(fp, "|  Reward:    %d\n", t->reward);
    fprintf(fp, " -------------------------- \n");
}

void print_block(block *b, FILE *fp)
{
    int i;
    transaction printable;
    fprintf(fp, "[BLOCK %d] ==========================================\n", b->blockIndex);
    for (i = 0; i < SO_BLOCK_SIZE; i++)
    {
        printable = b->transList[i];
        print_transaction(&printable, fp);
    }
    fprintf(fp, "=====================================================\n");
}

void print_ledger(block *l)
{
    int i = 0;
    long flag = 1;
    FILE *fp = fopen("ledger.txt", "w");

    fprintf(fp, "\n\
:::        :::::::::: :::::::::   ::::::::  :::::::::: :::::::::\n\
:+:        :+:        :+:    :+: :+:    :+: :+:        :+:    :+:\n\
+:+        +:+        +:+    +:+ +:+        +:+        +:+    +:+\n\
+#+        +#++:++#   +#+    +:+ :#:        +#++:++#   +#++:++#:\n\
+#+        +#+        +#+    +#+ +#+   +#+# +#+        +#+    +#+\n\
#+#        #+#        #+#    #+# #+#    #+# #+#        #+#    #+#\n\
########## ########## #########   ########  ########## ###    ###\n");
    for (i = 0; i < SO_REGISTRY_SIZE && flag != 0; i++)
    {
        flag = l[i].transList[0].timestamp.tv_nsec;

        if (flag)
            print_block(&l[i], fp);
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

    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
 */
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
        print_transaction(&printable, (FILE *)1);

        tmp->head = tmp->head->transactionMessage.next;
        i++;
    }
}

void print_outgoing_pool(struct node *outPool)
{
    struct node *tmp = outPool;
    int i = 0;
    pid_t pidCaller = getpid();
    transaction printable;

    printf("[USER %d] printing out pool:\n", pidCaller);
    while (tmp->next != NULL)
    {
        printf("[%d]", i);
        printable = tmp->trans;
        print_transaction(&printable, (FILE *)1);

        tmp = tmp->next;
        i++;
    }
}

/* 18 lines long */
void print_most_significant_processes(user *userPID, node *nodePID, struct parameters *par)
{
    int i;
    static pid_t n1, n2, n3, n4, n5, n6;               /* pid of the most significant nodes */
    static unsigned long bn1, bn2, bn3, bn4, bn5, bn6; /* balance of the ost significant nodes */
    static pid_t u1, u2, u3, u4, u5, u6;
    static unsigned int bu1, bu2, bu3, bu4, bu5, bu6;

    n1 = n2 = n3 = n4 = n5 = n6 = 0;
    bn1 = bn2 = bn3 = 0;
    bn4 = bn5 = bn6 = ULONG_MAX;
    u1 = u2 = u3 = u4 = u5 = u6 = 0;
    bu1 = bu2 = bu3 = 0;
    bu4 = bu5 = bu6 = UINT_MAX;

    for (i = 0; i < par->SO_NODES_NUM; i++)
    {
        if (nodePID[i].balance > bn1)
        {
            bn1 = nodePID[i].balance;
            n1 = nodePID[i].pid;
        }
        else if (nodePID[i].balance > bn2)
        {
            bn2 = nodePID[i].balance;
            n2 = nodePID[i].pid;
        }
        else if (nodePID[i].balance > bn3)
        {
            bn3 = nodePID[i].balance;
            n3 = nodePID[i].pid;
        }
        else if (nodePID[i].balance < bn6)
        {
            bn6 = nodePID[i].balance;
            n6 = nodePID[i].pid;
        }
        else if (nodePID[i].balance < bn5)
        {
            bn5 = nodePID[i].balance;
            n5 = nodePID[i].pid;
        }
        else if (nodePID[i].balance < bn4)
        {
            bn4 = nodePID[i].balance;
            n4 = nodePID[i].pid;
        }
    }

    for (i = 0; i < par->SO_USER_NUM; i++)
    {
        if (userPID[i].balance > bu1)
        {
            bu1 = userPID[i].balance;
            u1 = userPID[i].pid;
        }
        else if (userPID[i].balance > bu2)
        {
            bu2 = userPID[i].balance;
            u2 = userPID[i].pid;
        }
        else if (userPID[i].balance > bu3)
        {
            bu3 = userPID[i].balance;
            u3 = userPID[i].pid;
        }
        else if (userPID[i].balance < bu6)
        {
            bu6 = userPID[i].balance;
            u6 = userPID[i].pid;
        }
        else if (userPID[i].balance < bu5)
        {
            bu5 = userPID[i].balance;
            u5 = userPID[i].pid;
        }
        else if (userPID[i].balance < bu4)
        {
            bu4 = userPID[i].balance;
            u4 = userPID[i].pid;
        }
    }

    /* the extra spaces are to clear extra digits when updating */
    printf("[ Most significant nodes ]\n");
    printf("| %7d   (%lu UC)                           \n", n1, bn1);
    printf("| %7d   (%lu UC)                           \n", n2, bn2);
    printf("| %7d   (%lu UC)                           \n", n3, bn3);
    printf("| - - - - - - - - - - - -\n");
    printf("| %7d   (%lu UC)                           \n", n4, bn4);
    printf("| %7d   (%lu UC)                           \n", n5, bn5);
    printf("| %7d   (%lu UC)                           \n", n6, bn6);
    printf("\n[ Most significant users ]\n");
    printf("| %7d   (%u UC)                 \n", u1, bu1);
    printf("| %7d   (%u UC)                 \n", u2, bu2);
    printf("| %7d   (%u UC)                 \n", u3, bu3);
    printf("| - - - - - - - - - - - -\n");
    printf("| %7d   (%u UC)                 \n", u4, bu4);
    printf("| %7d   (%u UC)                 \n", u5, bu5);
    printf("| %7d   (%u UC)                 \n\n", u6, bu6);
}
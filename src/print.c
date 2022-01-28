
#include "include/common.h"
#include "include/print.h"

void print_user_nodes_table(pid_t mainPID, user *userPID, node *nodePID, struct parameters *par)
{
    int userNum = par->SO_USER_NUM;
    int nodesNum = 0;
    int statusNum = 0;
    char statusStr[19];

    printf(" -----------------------------------------------------------------\n|                 Master Process PID is %7d                   |\n", mainPID);
    printf("|                                                                 |\n");
    printf(" - Type ----- PID ----- Status ----- Balance ---------------------\n");
    printf(" -----------------------------------------------------------------\n");
    while (userNum--)
    {
        statusNum = userPID[userNum].status;
        switch (statusNum)
        {
        case 0:
            strcpy(statusStr, "\033[32malive\033[0m    ");
            break;
        case 1:
            strcpy(statusStr, "\033[33mbroke\033[0m    ");
            break;
        case 2:
            strcpy(statusStr, "\033[31mdead\033[0m     ");
            break;
        }

        /* we should place it in a buffer so that they print a fixed length */
        printf("|  User      %7d    %s  %10u                     |\n", userPID[userNum].pid, statusStr, userPID[userNum].balance);
    }
    printf("|                                                                 |\n");
    printf(" - Type ----- PID ----- Status ----- Balance ----- Still in Pool -\n");
    printf(" -----------------------------------------------------------------\n");
    while (nodesNum < par->SO_NODES_NUM * 2 && nodePID[nodesNum].pid != 0)
    {
        statusNum = nodePID[nodesNum].status;
        switch (statusNum)
        {
        case 0:
            strcpy(statusStr, "\033[32mavailable\033[0m");
            break;
        case 1:
            strcpy(statusStr, "\033[33mfull\033[0m     ");
            break;
        }
        printf("|  Node      %7d    %s  %10lu         %3d         |\n", nodePID[nodesNum].pid, statusStr, nodePID[nodesNum].balance, nodePID[nodesNum].tpSize);
        nodesNum++;
    }
    printf(" -----------------------------------------------------------------\n");
}

void print_num_aborted(user *usersPID, struct parameters *par)
{
    int i;
    int counter = 0;

    for (i = 0; i < par->SO_USER_NUM; i++)
    {
        if (usersPID[i].status == dead)
            counter++;
    }
    printf("%d users have died prematurely\n", counter);
}

int print_num_blocks(block *l)
{
    int i = 0;
    int blockIndex = 0;

    while (l[i].transList[0].sender == -1 && i < SO_REGISTRY_SIZE)
    {
        blockIndex = l[i].blockIndex;
        i++;
    }
    printf("%d blocks have been confirmed on ledger\n", blockIndex);
    return blockIndex;
}

void print_kill_signal(enum term termReason)
{
    switch (termReason)
    {
    case timeElapsed:
        printf("KILL signal: time elapsed or interrupted by user\n");
        break;
    case usersDead:
        printf("KILL signal: all or all-1 users dead\n");
        break;
    case ledgerFull:
        printf("KILL signal: ledger full\n");
        break;
    default:
        printf("%s/%d strange error\n", __FILE__, __LINE__);
        break;
    }
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
    char ts[31];
    char tmp[11];
    switch (t->status)
    {
    case pending:
        strcpy(tmp, "pending   ");
        break;
    case aborted:
        strcpy(tmp, "aborted   ");
        break;
    case confirmed:
        strcpy(tmp, "confirmed ");
        break;
    case processing:
        strcpy(tmp, "processing");
        break;
    }
    formatted_timestamp(ts, t->timestamp);

    fprintf(fp, " ---------------------------------------------------------------\n");
    fprintf(fp, "|  %s                                |\n", ts);
    fprintf(fp, "|  %s                                                   |\n", tmp);
    fprintf(fp, "|  %7d --> %7d                                          |\n", t->sender, t->receiver);
    fprintf(fp, "|  Amount:    %10u                                        |\n", t->amount);
    fprintf(fp, "|  Reward:    %10u                                        |\n", t->reward);
    fprintf(fp, " ---------------------------------------------------------------\n");
}

void print_block(block *b, FILE *fp)
{
    int i;
    transaction printable;
    fprintf(fp, "[BLOCK %5d] ===================================================\n", b->blockIndex);
    for (i = 0; i < SO_BLOCK_SIZE; i++)
    {
        printable = b->transList[i];
        print_transaction(&printable, fp);
    }
    fprintf(fp, "=================================================================\n");
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
void formatted_timestamp(char *tsString, struct timespec timestamp)
{
    struct tm time;
    gmtime_r(&timestamp.tv_sec, &time);
    strftime(tsString, 31 - 10, "%Y-%m-%d %H:%M:%S.", &time);
    sprintf(tsString + strlen(tsString), "%09lu", timestamp.tv_nsec);
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
void print_most_significant_processes(user *usersPID, node *nodesPID, struct parameters *par)
{
    int i;
    static pid_t n1 = 0;
    static pid_t n2 = 0;
    static pid_t n3 = 0;
    static pid_t n4 = 0;
    static pid_t n5 = 0;
    static pid_t n6 = 0;
    unsigned long bn1 = 0;
    unsigned long bn2 = 0;
    unsigned long bn3 = 0;
    unsigned long bn4 = ULONG_MAX;
    unsigned long bn5 = ULONG_MAX;
    unsigned long bn6 = ULONG_MAX;
    pid_t u1 = 0;
    pid_t u2 = 0;
    pid_t u3 = 0;
    pid_t u4 = 0;
    pid_t u5 = 0;
    pid_t u6 = 0;
    unsigned int bu1 = 0;
    unsigned int bu2 = 0;
    unsigned int bu3 = 0;
    unsigned int bu4 = UINT_MAX;
    unsigned int bu5 = UINT_MAX;
    unsigned int bu6 = UINT_MAX;

    /* 4 for loops instead of 2 to avoid seeig ULONG_MAX, especially for nodes */
    for (i = 0; i < par->SO_NODES_NUM; i++)
    {
        if (nodesPID[i].balance > bn1)
        {
            bn2 = bn1;
            n2 = n1;

            bn1 = nodesPID[i].balance;
            n1 = nodesPID[i].pid;
        }
        else if (nodesPID[i].balance > bn2)
        {
            bn3 = bn2;
            n3 = n2;

            bn2 = nodesPID[i].balance;
            n2 = nodesPID[i].pid;
        }
        else if (nodesPID[i].balance > bn3)
        {
            bn3 = nodesPID[i].balance;
            n3 = nodesPID[i].pid;
        }
    }

    for (i = 0; i < par->SO_NODES_NUM; i++)
    {
        if (nodesPID[i].balance < bn6)
        {
            bn5 = bn6;
            n5 = n6;

            bn6 = nodesPID[i].balance;
            n6 = nodesPID[i].pid;
        }
        else if (nodesPID[i].balance < bn5)
        {
            bn4 = bn5;
            n4 = n5;

            bn5 = nodesPID[i].balance;
            n5 = nodesPID[i].pid;
        }
        else if (nodesPID[i].balance < bn4)
        {
            bn4 = nodesPID[i].balance;
            n4 = nodesPID[i].pid;
        }
    }

    for (i = 0; i < par->SO_USER_NUM; i++)
    {
        if (usersPID[i].balance > bu1)
        {
            bn2 = bn1;
            u2 = u1;

            bu1 = usersPID[i].balance;
            u1 = usersPID[i].pid;
        }
        else if (usersPID[i].balance > bu2)
        {
            bu3 = bu2;
            u3 = u2;

            bu2 = usersPID[i].balance;
            u2 = usersPID[i].pid;
        }
        else if (usersPID[i].balance > bu3)
        {
            bu3 = usersPID[i].balance;
            u3 = usersPID[i].pid;
        }
    }

    for (i = 0; i < par->SO_USER_NUM; i++)
    {
        if (usersPID[i].balance < bu6)
        {
            bu5 = bu6;
            u5 = u6;

            bu6 = usersPID[i].balance;
            u6 = usersPID[i].pid;
        }
        else if (usersPID[i].balance < bu5)
        {
            bu4 = bu5;
            u4 = u5;

            bu5 = usersPID[i].balance;
            u5 = usersPID[i].pid;
        }
        else if (usersPID[i].balance < bu4)
        {
            bu4 = usersPID[i].balance;
            u4 = usersPID[i].pid;
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
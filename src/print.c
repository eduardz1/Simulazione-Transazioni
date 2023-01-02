
#include "include/print.h"
#include "include/common.h"
#include "utils/debug.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void print_user_nodes_table(pid_t mainPID,
                            User *userPID,
                            Node *nodePID,
                            struct Params *par)
{
    float finalBalance = 0;
    uint32_t userNum = par->SO_USER_NUM;
    uint32_t nodesNum = 0;
    int statusNum;
    char statusStr[19];

    printf(" -----------------------------------------------------------------"
           "\n|                 Master Process PID is %7d                   "
           "|\n",
           mainPID);
    printf("|                                                                 "
           "|\n");
    printf(" - Type ----- PID ----- Status ----- Balance "
           "---------------------\n");
    printf(" -----------------------------------------------------------------"
           "\n");
    while (userNum--)
    {
        finalBalance += userPID[userNum].balance;
        statusNum = userPID[userNum].status;
        switch (statusNum)
        {
        case 0: strcpy(statusStr, "\033[32malive\033[0m    "); break;
        case 1: strcpy(statusStr, "\033[33mbroke\033[0m    "); break;
        case 2: strcpy(statusStr, "\033[31mdead\033[0m     "); break;
        }

        /* we should place it in a buffer so that they print a fixed length */
        printf("|  User      %7d    %s  %10u\033[34mUC\033[0m                  "
               " |\n",
               userPID[userNum].pid,
               statusStr,
               userPID[userNum].balance);
    }
    printf("|                                                                 "
           "|\n");
    printf(" - Type ----- PID ----- Status ----- Balance ----- Still in Pool "
           "-\n");
    printf(" -----------------------------------------------------------------"
           "\n");
    while (nodesNum < par->SO_NODES_NUM * 2 && nodePID[nodesNum].pid != 0)
    {
        finalBalance += nodePID[nodesNum].balance;
        statusNum = nodePID[nodesNum].status;
        switch (statusNum)
        {
        case 0: strcpy(statusStr, "\033[32mavailable\033[0m"); break;
        case 1: strcpy(statusStr, "\033[33mfull\033[0m     "); break;
        }
        printf("|  Node      %7d    %s  %10lu\033[34mUC\033[0m         %3d     "
               "  |\n",
               nodePID[nodesNum].pid,
               statusStr,
               nodePID[nodesNum].balance,
               nodePID[nodesNum].tpSize);
        nodesNum++;
    }
    printf(" -----------------------------------------------------------------"
           "\n");
    TRACE(("-- Percent diff %f\n",
           (100 * finalBalance) / (par->SO_USER_NUM * par->SO_BUDGET_INIT)))
}

void print_num_aborted(User *usersPID, struct Params *par)
{
    uint32_t i, counter = 0;

    for (i = 0; i < par->SO_USER_NUM; i++)
    {
        if (usersPID[i].status == dead) counter++;
    }
    printf("%d users have died prematurely\n", counter);
}

int print_num_blocks(Ledger *l)
{
    size_t i;
    for (i = 0;
         l->blockList[i].transList[0].sender == -1 && i < SO_REGISTRY_SIZE;
         i++)
        ;
    printf("%lu blocks have been confirmed on ledger\n", i + 1);
    return i;
}

void print_kill_signal(enum Term termReason)
{
    switch (termReason)
    {
    case TIME_ELAPSED:
        printf("KILL signal: time elapsed or interrupted by user\n");
        break;
    case USER_DEAD: printf("KILL signal: all or all-1 users dead\n"); break;
    case LEDGER_FULL: printf("KILL signal: ledger full\n"); break;
    default: printf("%s/%d strange error\n", __FILE__, __LINE__); break;
    }
}

void print_parameters(struct Params *par)
{
    printf("--------------------------------------------\n----------- "
           "Configuration value ------------\n");
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

void print_transaction(Transaction *t, FILE *fp)
{
    char ts[31];
    char tmp[11];
    switch (t->status)
    {
    case PENDING: strcpy(tmp, "pending   "); break;
    case ABORTED: strcpy(tmp, "aborted   "); break;
    case CONFIRMED: strcpy(tmp, "confirmed "); break;
    case PROCESSING: strcpy(tmp, "processing"); break;
    }
    formatted_timestamp(ts, t->timestamp);

    fprintf(fp,
            " ---------------------------------------------------------------"
            "\n");
    fprintf(fp, "|  %s                                |\n", ts);
    fprintf(
        fp, "|  %s                                                   |\n", tmp);
    fprintf(fp,
            "|  %7d --> %7d                                          |\n",
            t->sender,
            t->receiver);
    fprintf(fp,
            "|  Amount:    %10u                                        |\n",
            t->amount);
    fprintf(fp,
            "|  Reward:    %10u                                        |\n",
            t->reward);
    fprintf(fp,
            " ---------------------------------------------------------------"
            "\n");
}

void print_block(Block *b, FILE *fp, size_t index)
{
    Transaction printable;
    fprintf(fp,
            "[BLOCK %5lu] "
            "===================================================\n",
            index);
    for (size_t i = 0; i < SO_BLOCK_SIZE; i++)
    {
        printable = b->transList[i];
        print_transaction(&printable, fp);
    }
    fprintf(fp,
            "================================================================="
            "\n");
}

void print_ledger(Ledger *l)
{
    FILE *fp = fopen("ledger.txt", "w");

    fprintf(fp, "\n\
:::        :::::::::: :::::::::   ::::::::  :::::::::: :::::::::\n\
:+:        :+:        :+:    :+: :+:    :+: :+:        :+:    :+:\n\
+:+        +:+        +:+    +:+ +:+        +:+        +:+    +:+\n\
+#+        +#++:++#   +#+    +:+ :#:        +#++:++#   +#++:++#:\n\
+#+        +#+        +#+    +#+ +#+   +#+# +#+        +#+    +#+\n\
#+#        #+#        #+#    #+# #+#    #+# #+#        #+#    #+#\n\
########## ########## #########   ########  ########## ###    ###\n");
    for (size_t i = 0; i < SO_REGISTRY_SIZE; i++)
    {
        if (!l->blockList[i].transList[0].timestamp.tv_nsec) break;
        print_block(&l->blockList[i], fp, i);
    }

    fclose(fp);
}

/* print without /n */
void formatted_timestamp(char *tsString, struct timespec timestamp)
{
    struct tm time = {0};
    gmtime_r(&timestamp.tv_sec, &time);
    strftime(tsString, 31 - 10, "%Y-%m-%d %H:%M:%S.", &time);
    sprintf(tsString + strlen(tsString), "%09lu", timestamp.tv_nsec);
}

/* 18 lines long */
void print_most_significant_processes(User *usersPID,
                                      Node *nodesPID,
                                      struct Params *par)
{
    uint32_t i;
    static pid_t n1 = 0;
    static pid_t n2 = 0;
    static pid_t n3 = 0;
    static pid_t n4 = 0;
    static pid_t n5 = 0;
    static pid_t n6 = 0;
    uint64_t bn1 = 0;
    uint64_t bn2 = 0;
    uint64_t bn3 = 0;
    uint64_t bn4 = ULONG_MAX;
    uint64_t bn5 = ULONG_MAX;
    uint64_t bn6 = ULONG_MAX;
    pid_t u1 = 0;
    pid_t u2 = 0;
    pid_t u3 = 0;
    pid_t u4 = 0;
    pid_t u5 = 0;
    pid_t u6 = 0;
    uint32_t bu1 = 0;
    uint32_t bu2 = 0;
    uint32_t bu3 = 0;
    uint32_t bu4 = UINT_MAX;
    uint32_t bu5 = UINT_MAX;
    uint32_t bu6 = UINT_MAX;

    /* 4 for loops instead of 2 to avoid seeig ULONG_MAX, especially for nodes
     */
    for (i = 0; i < par->SO_NODES_NUM * 2; i++)
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

    for (i = 0; i < par->SO_NODES_NUM * 2; i++)
    {
        if (nodesPID[i].balance < bn6 && nodesPID[i].pid != 0)
        {
            bn5 = bn6;
            n5 = n6;

            bn6 = nodesPID[i].balance;
            n6 = nodesPID[i].pid;
        }
        else if (nodesPID[i].balance < bn5 && nodesPID[i].pid != 0)
        {
            bn4 = bn5;
            n4 = n5;

            bn5 = nodesPID[i].balance;
            n5 = nodesPID[i].pid;
        }
        else if (nodesPID[i].balance < bn4 && nodesPID[i].pid != 0)
        {
            bn4 = nodesPID[i].balance;
            n4 = nodesPID[i].pid;
        }
    }

    for (i = 0; i < par->SO_USER_NUM; i++)
    {
        if (usersPID[i].balance > bu1)
        {
            bu2 = bu1;
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
    printf("| %7d   %10lu\033[34mUC\033[0m |\n", n1, bn1);
    printf("| %7d   %10lu\033[34mUC\033[0m |\n", n2, bn2);
    printf("| %7d   %10lu\033[34mUC\033[0m |\n", n3, bn3);
    printf("| - - - - - - - - - - - -|\n");
    printf("| %7d   %10lu\033[34mUC\033[0m |\n", n4, bn4);
    printf("| %7d   %10lu\033[34mUC\033[0m |\n", n5, bn5);
    printf("| %7d   %10lu\033[34mUC\033[0m |\n", n6, bn6);
    printf("\n[ Most significant users ]\n");
    printf("| %7d   %10u\033[34mUC\033[0m |\n", u1, bu1);
    printf("| %7d   %10u\033[34mUC\033[0m |\n", u2, bu2);
    printf("| %7d   %10u\033[34mUC\033[0m |\n", u3, bu3);
    printf("| - - - - - - - - - - - -|\n");
    printf("| %7d   %10u\033[34mUC\033[0m |\n", u4, bu4);
    printf("| %7d   %10u\033[34mUC\033[0m |\n", u5, bu5);
    printf("| %7d   %10u\033[34mUC\033[0m |\n\n", u6, bu6);
}

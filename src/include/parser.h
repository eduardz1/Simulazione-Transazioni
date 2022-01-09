#ifndef SIMULAZIONE_TRANSAZIONI_PARSER_H
#define SIMULAZIONE_TRANSAZIONI_PARSER_H

#include <stdio.h>
#include <string.h>

#define NUM_PARAMETERS 13
#define CONF_FILE "../conf.txt"

struct parameters
{
    int SO_USER_NUM = 100;
    int SO_NODES_NUM = 10;
    int SO_BUDGET_INIT = 1000;
    int SO_REWARD = 1;
    int SO_MIN_TRANS_GEN_NSEC = 100000000;
    int SO_MAX_TRANS_GEN_NSEC = 200000000;
    int SO_RETRY = 20;
    int SO_TP_SIZE = 1000;
    int SO_MIN_TRANS_PROC_NSEC = 100000000;
    int SO_MAX_TRANS_PROC_NSEC = 200000000;
    int SO_SIM_SEC = 10;
    int SO_FRIENDS_NUM = 3;
    int SO_HOPS = 10;
};

struct parameters parser(void);

#endif /* SIMULAZIONE_TRANSAZIONI_PARSER_H */
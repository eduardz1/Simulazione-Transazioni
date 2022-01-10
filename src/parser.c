#include "include/common.h"
#include "include/parser.h"

/*enum paramID
{
    SO_USER_NUM,
    SO_NODES_NUM,
    SO_NUM_FRIENDS,
    SO_SIM_SEC,
    SO_HOPS,
    SO_BUDGET_INIT,
    SO_REWARD,
    SO_MIN_TRANS_GEN_NSEC,
    SO_MAX_TRANS_GEN_NSEC,
    SO_RETRY,
    SO_TP_SIZE,
    SO_MIN_TRANS_PROC_NSEC,
    SO_MAX_TRANS_PROC_NSEC
};

struct parameters
{
    char *string;
    enum paramID id;
} paramList[] = {
    {"SO_USER_NUM", SO_USER_NUM},
    {"SO_NODES_NUM", SO_NODES_NUM},
    {"SO_NUM_FRIENDS", SO_NUM_FRIENDS},
    {"SO_SIM_SEC", SO_SIM_SEC},
    {"SO_HOPS", SO_HOPS},
    {"SO_BUDGET_INIT", SO_BUDGET_INIT},
    {"SO_REWARD", SO_REWARD},
    {"SO_MIN_TRANS_GEN_NSEC", SO_MIN_TRANS_GEN_NSEC},
    {"SO_MAX_TRANS_GEN_NSEC", SO_MAX_TRANS_GEN_NSEC},
    {"SO_RETRY", SO_RETRY},
    {"SO_TP_SIZE", SO_TP_SIZE},
    {"SO_MIN_TRANS_PROC_NSEC", SO_MIN_TRANS_PROC_NSEC},
    {"SO_MAX_TRANS_PROC_NSEC", SO_MAX_TRANS_PROC_NSEC}};*/


struct parameters parser(void)
{
    FILE *fp;

    /*enum paramID tokensE;*/

    /* longer than NUM_PARAMETERS to account for comments and such */
    char buffer[128];
    int i = 0;

    struct parameters *par;
    /* con#1 as default *
    int parameters[NUM_PARAMETERS] = {100,
                                      10,
                                      1000,
                                      1,
                                      100000000,
                                      200000000,
                                      20,
                                      1000,
                                      100000000,
                                      200000000,
                                      10,
                                      3,
                                      10};*/

    char *tokens[NUM_PARAMETERS];
    int values[NUM_PARAMETERS];

    fp = fopen(CONF_FILE, "r");
    if (fp == NULL)
        printf("oops");
    /* return parameters; /* default config */

    while (fgets(buffer, 127, fp))
    {
        tokens[i] = malloc(64);
        /*values[i] = malloc(sizeof(int));*/

        sscanf(buffer, "%s %d", tokens[i], &values[i]);

        i++;
    }

    for (i = 0; i < NUM_PARAMETERS; i++)
    {
        /*switch(tokensE){
            case SO_USER_NUM:
            case SO_NODES_NUM:
            case SO_NUM_FRIENDS:
            case SO_SIM_SEC:
            case SO_HOPS:
            case SO_BUDGET_INIT:
            case SO_REWARD:
            case SO_MIN_TRANS_GEN_NSEC:
            case SO_MAX_TRANS_GEN_NSEC:
            case SO_RETRY:
            case SO_TP_SIZE:
            case SO_MIN_TRANS_PROC_NSEC:
            case SO_MAX_TRANS_PROC_NSEC:
            default:
            break;
        } it can be implemented in a nicer to look at way, but not now */

        /*printf("%s\n",tokens[i]);*/
        
        if (!strcmp(tokens[i], "SO_USER_NUM"))
        {
            par->SO_USER_NUM = values[i];
        }
        else if (!strcmp(tokens[i], "SO_NODES_NUM"))
        {
            par->SO_NODES_NUM = values[i];
        }
        else if (!strcmp(tokens[i], "SO_BUDGET_INIT"))
        {
            par->SO_BUDGET_INIT = values[i];
        }
        else if (!strcmp(tokens[i], "SO_REWARD"))
        {
            if (values[i] >= 0 && values[i] <= 100)
            {
                par->SO_REWARD = values[i];
            }
            else
            {
                printf("-- SO_REWARD incorrect value, defaulting to 1\n");
            }
        }
        else if (!strcmp(tokens[i], "SO_MIN_TRANS_GEN_NSEC"))
        {
            par->SO_MIN_TRANS_GEN_NSEC = values[i];
        }
        else if (!strcmp(tokens[i], "SO_MAX_TRANS_GEN_NSEC"))
        {
            par->SO_MAX_TRANS_GEN_NSEC = values[i];
        }
        else if (!strcmp(tokens[i], "SO_RETRY"))
        {
            par->SO_RETRY = values[i];
        }
        else if (!strcmp(tokens[i], "SO_TP_SIZE"))
        {
            par->SO_TP_SIZE = values[i];
        }
        else if (!strcmp(tokens[i], "SO_MIN_TRANS_PROC_NSEC"))
        {
            par->SO_MIN_TRANS_PROC_NSEC = values[i];
        }
        else if (!strcmp(tokens[i], "SO_MAX_TRANS_PROC_NSEC"))
        {
            par->SO_MAX_TRANS_PROC_NSEC = values[i];
        }
        else if (!strcmp(tokens[i], "SO_SIM_SEC"))
        {
            par->SO_SIM_SEC = values[i];
        }
        else if (!strcmp(tokens[i], "SO_FRIENDS_NUM"))
        {
            par->SO_FRIENDS_NUM = values[i];
        }
        else if (!strcmp(tokens[i], "SO_HOPS"))
        {
            par->SO_HOPS = values[i];
        }

        /*printf("end check\n");*/

        /*free(tokens[i]);*/
        /*free(values[i]);*/
    }

    for (i = 0; i < NUM_PARAMETERS; i++)
    {
        /*printf("%s %d\n", tokens[i], values[i]);*/
        /*printf("%d\n", parameters[i]);*/
        free(tokens[i]);  
    }

    fclose(fp);

    return *par;
}
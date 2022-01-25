#include "include/common.h"
#include "utils/debug.h"
#include "include/parser.h"

#define CONF_FILE "conf1.txt"

void assign_defaults(struct parameters *par)
{
    par->SO_USER_NUM = 100;
    par->SO_NODES_NUM = 10;
    par->SO_BUDGET_INIT = 1000;
    par->SO_REWARD = 1;
    par->SO_MIN_TRANS_GEN_NSEC = 100000000;
    par->SO_MAX_TRANS_GEN_NSEC = 200000000;
    par->SO_RETRY = 20;
    par->SO_TP_SIZE = 1000;
    par->SO_MIN_TRANS_PROC_NSEC = 100000000;
    par->SO_MAX_TRANS_PROC_NSEC = 200000000;
    par->SO_SIM_SEC = 10;
    par->SO_FRIENDS_NUM = 3;
    par->SO_HOPS = 10;
}

int parse_parameters(struct parameters *par)
{
    FILE *fp;

    /*enum paramID tokensE;*/

    /* longer than NUM_PARAMETERS to account for comments and such */
    char buffer[128];
    int i = 0;

    char *tokens[NUM_PARAMETERS];
    unsigned long values[NUM_PARAMETERS]; /* downcast is easy, upcast not so much */

    /*struct parameters *par = malloc(sizeof(struct parameters));*/
    assign_defaults(par);
    TRACE(("[PARSER] assigned defaults\n"));

    fp = fopen(CONF_FILE, "r");
    if (fp == NULL)
        return CONF_ERROR; /* default config */

    while (fgets(buffer, 127, fp))
    {
        tokens[i] = malloc(64);
        /*values[i] = malloc(sizeof(int));*/

        sscanf(buffer, "%s %lu", tokens[i], &values[i]);

        i++;
    }

    for (i = 0; i < NUM_PARAMETERS; i++)
    {
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
            /* given that it is a char it's very easy to get it out of bound,
             * I prefer straight up normalizing it rather than resetting everything
             * because of ERANGE
             */
            if (values[i] >= 0 && values[i] <= 100)
                par->SO_REWARD = values[i];
            else
                printf("[PARSER] SO_REWARD incorrect value, resetting default\n");
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
    }

    /* -- CONF ERRORS CORRECTION -- */
    if (errno == ERANGE)
    {
        TRACE(("[PARSER] one or multiple values out of bound, resetting defaults\n"));
        assign_defaults(par);
    }
    if (par->SO_MIN_TRANS_GEN_NSEC > par->SO_MAX_TRANS_GEN_NSEC)
    {
        TRACE(("[PARSER] SO_MIN_TRANS_GEN_NSEC greater than SO_MAX_TRANS_GEN_NSEC, will be normalized\n"));
        par->SO_MIN_TRANS_GEN_NSEC = par->SO_MAX_TRANS_GEN_NSEC;
    }
    if (par->SO_MIN_TRANS_PROC_NSEC > par->SO_MAX_TRANS_PROC_NSEC)
    {
        TRACE(("[PARSER] SO_MIN_TRANS_PROC_NSEC greater than SO_MAX_TRANS_PROC_NSEC, will be normalized\n"));
        par->SO_MIN_TRANS_PROC_NSEC = par->SO_MAX_TRANS_PROC_NSEC;
    }
    if(par->SO_TP_SIZE <= SO_BLOCK_SIZE){
        TRACE(("[PARSER] SO_TP_SIZE smaller or equal to SO_BLOCK_SIZE, will be normalized\n"));
        par->SO_TP_SIZE = SO_BLOCK_SIZE + 1;
    }

    TRACE(("--------------------------------------------\n----------- Configuration input ------------\n"));
    for (i = 0; i < NUM_PARAMETERS; i++)
    {
        TRACE(("%s %lu\n", tokens[i], values[i]));
        free(tokens[i]);
    }
    TRACE(("--------------------------------------------\n"));

    fclose(fp);

    return 0;
}
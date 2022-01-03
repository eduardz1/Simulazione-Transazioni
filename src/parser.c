#include <stdio.h>
#include <string.h>

#include "include/common.h"

#define NUM_PARAMETERS 13
#define BUF_SIZE 128
#define CONF_FILE "conf.txt"
#define DELIMITER "="


/*** veeeery work in progress ***/
int *parser(int argc, char **argv)
{
    FILE *fp;

    /* longer than NUM_PARAMETERS to account for comments and such */
    char buffer[BUF_SIZE] = {0};
    int lines = 0, i = 0;

    char *token;

    /* con#1 as default */
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
                                      10};

    fp = fopen(CONF_FILE, "r");
    if (fp == NULL)
        return parameters;

    while (fgets(buffer, BUF_SIZE, fp) != NULL)
    {
        lines++;
    }

    token = strtok(buffer, DELIMITER);
    while(token!=NULL){
        switch(strcmp(token[0])){
            case '#':
            break;

            case 
        }

        token = strtok(NULL, DELIMITER);
    }

    /*for (i; i < lines; i++)
    {
    }*/
}

char *read_line(FILE *fp, char *parameter){

}
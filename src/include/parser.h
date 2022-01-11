#ifndef SIMULAZIONE_TRANSAZIONI_PARSER_H
#define SIMULAZIONE_TRANSAZIONI_PARSER_H

#include <stdio.h>
#include <string.h>

#define NUM_PARAMETERS 13
#define CONF_FILE "conf.txt"
#define CONF_ERROR -1

int parse_parameters(struct parameters *par);

void assign_defaults(struct parameters *par);

#endif /* SIMULAZIONE_TRANSAZIONI_PARSER_H */
#ifndef SIMULAZIONE_TRANSAZIONI_PARSER_H
#define SIMULAZIONE_TRANSAZIONI_PARSER_H

#include <stdio.h>
#include <string.h>
#include "../include/Common.h"
#define NUM_PARAMETERS 13
#define CONF_ERROR -1
/* Prototype */
void assign_defaults(configparameter *par);
int parse_parameters(configparameter *par);

#endif /* SIMULAZIONE_TRANSAZIONI_PARSER_H */

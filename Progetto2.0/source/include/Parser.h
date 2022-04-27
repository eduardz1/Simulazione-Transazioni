#ifndef SIMULAZIONE_TRANSAZIONI_PARSER_H
#define SIMULAZIONE_TRANSAZIONI_PARSER_H

#include <stdio.h>
#include <string.h>
#include "Common.h"
#define NUM_PARAMETERS 13
#define CONF_ERROR -1
/* Prototype */
void assign_defaults(struct ConfigParameters *par); 
int parse_parameters(struct ConfigParameters *par); 

#endif /* SIMULAZIONE_TRANSAZIONI_PARSER_H */

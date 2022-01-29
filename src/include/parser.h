#ifndef SIMULAZIONE_TRANSAZIONI_PARSER_H
#define SIMULAZIONE_TRANSAZIONI_PARSER_H

#include <stdio.h>
#include <string.h>

#define NUM_PARAMETERS 13
#define CONF_ERROR -1
/* Prototype*/
void assign_defaults(struct parameters *par); 
int parse_parameters(struct parameters *par); 

#endif /* SIMULAZIONE_TRANSAZIONI_PARSER_H */
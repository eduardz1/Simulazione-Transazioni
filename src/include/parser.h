#ifndef SIMULAZIONE_TRANSAZIONI_PARSER_H
#define SIMULAZIONE_TRANSAZIONI_PARSER_H

#include "common.h"
#include <stdio.h>
#include <string.h>

#define NUM_PARAMETERS 13

void assign_defaults(struct Params *par);
int parse_parameters(struct Params *par);

#endif /* SIMULAZIONE_TRANSAZIONI_PARSER_H */

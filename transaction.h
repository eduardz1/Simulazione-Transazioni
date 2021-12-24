#ifndef SIMULAZIONE_TRANSAZIONI_TRANSACTIONS_H
#define SIMULAZIONE_TRANSAZIONI_TRANSACTIONS_H

#define SO_HOPS
#include <sys/types.h>

typedef struct transaction
{
	struct  timestamp;
	pid_t sender;
	pid_t  receiver;
	int  quantity;
	int reward;


}transaction;

#endif /*TRANSACTIONS */

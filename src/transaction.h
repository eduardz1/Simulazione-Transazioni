#ifndef SIMULAZIONE_TRANSAZIONI_TRANSACTIONS_H
#define SIMULAZIONE_TRANSAZIONI_TRANSACTIONS_H

/*
#ifndef
#define SO_HOPS 10  ~config #1 value
#endif
*/

#include <time.h>
#include <sys/types.h>

/*
 * timespec is a struct with an int to represent the seconds
 * and a long to represent the nanoseconds
 */
typedef struct transaction{
	timespec  timestamp;
	pid_t sender;
	pid_t  receiver;
	int  quantity;
	int reward;
};

transaction sendTransaction(pid_t sender, pid_t receiver, int quantity, int reward);

#endif /* SIMULAZIONE_TRANSAZIONI_TRANSACTIONS_H */

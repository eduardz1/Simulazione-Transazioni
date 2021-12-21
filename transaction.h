#define SO_HOPS
#include <sys/types.h>

typedef struct transaction
{
	struct  timestamp;
	pid_t sender;
	pid_t  receiver;
	int  quantity;
	int reward;


} transaction;

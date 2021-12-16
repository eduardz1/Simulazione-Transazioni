#define SO_HOPS
#include <sys/types.h>

struct Transaction
{
	struct  timestamp;
	pid_t sender;
	pid_t  receiver;
	int  quantity;
	int reward;


};

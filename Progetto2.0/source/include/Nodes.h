#include "Common.h"
#include <stdlib.h>

/*user return status, used in tp */
#define WENT_BROKE 1
#define MAX_RETRY 2
#define ERROR -1
#define SUCCESS 0

/*message buffer stored in transaction pool, contains message struct in order to define it*/
struct msgbuf_trans {
 struct message{
    int hops;
    transaction userTrans;
    struct msgbuf_trans *next;
  }transactionMessage;
};
typedef struct tp_pool {
    struct msgbuf_trans *head;
    struct msgbuf_trans *tail;
    unsigned int size;
}pool;

void transaction_pool_init(pool *transPool);
int add_to_pool(pool *transPool,struct msgbuf_trans *message);
int remove_from_pool(pool *transPool, struct msgbg_trans *message);

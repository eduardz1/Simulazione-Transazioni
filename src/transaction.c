#include "include/transaction.h"

struct transaction send_transaction(pid_t sender, pid_t receiver, int quantity, int reward)
{
    struct timespec tp;
    struct transaction currentTransaction;
    /*
     * get current time and save it in a struct timespec
     * inside the transaction struct
     */

    clock_gettime(CLOCK_REALTIME, &tp);

    currentTransaction.timestamp = tp;

    return currentTransaction;
}

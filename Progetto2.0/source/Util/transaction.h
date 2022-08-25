#include <sys/types.h>
#include <sys/time.h>

typedef struct MoneyTransaction
{
    struct timespec time;
    pid_t Sender; 
    pid_t Receiver; 
    int Money; 
    int Reward; 
    enum T_status
    { 
       pending,
       processing,
       confirmed,
       aborted 
 }MoneyStatusTrans; 
 
}transaction;
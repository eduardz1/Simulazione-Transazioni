#include "./include/Master.h"
#include "./include/Users.h"
#include "./include/Nodes.h"
#define MSGSZ 100

// msgp structure
struct msgbuf {
long mtype ; /* msg type - must be >0 */
char mtext [ MSGSZ ]; /* msg data */

};

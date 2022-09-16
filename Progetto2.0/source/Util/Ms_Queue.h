#include  <stddef.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
const void *Message_p; 
size_t Message_size; 
 
/*PROTOTYPE'S */
void Message_Queue();

int send_message(int queueID,void *msg,int size,int flag);
int receive_message(int queueID,void *msg,int size,int mtype,int flag);
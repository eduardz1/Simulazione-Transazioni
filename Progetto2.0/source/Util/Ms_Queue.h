#include  <stddef.h>
#include <errno.h>
const void *Message_p; 
size_t Message_size; 
 







/*PROTOTYPE'S */
void Message_Queue();

int send_message(int queueID,void *msg,int size,int flag);
int receive_message(int queueID,void *msg,int size,int mtype,int flag);

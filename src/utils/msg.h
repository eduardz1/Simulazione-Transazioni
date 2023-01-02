#ifndef SIMULAZIONE_TRANSAZIONI_MSG_H
#define SIMULAZIONE_TRANSAZIONI_MSG_H

#include <stddef.h>

int send_message(int queueID, const void *msg, size_t size, int flag);
int receive_message(int queueID, void *msg, size_t size, long mtype, int flag);

#endif /* SIMULAZIONE_TRANSAZIONI_MSG_H */

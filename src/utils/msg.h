#ifndef SIMULAZIONE_TRANSAZIONI_MSG_H
#define SIMULAZIONE_TRANSAZIONI_MSG_H

#include "../include/common.h"

int send_message(int queueID, void *msg,int size, int waitFlag);
int receive_message(int queueID, void *msg, int size, int mtype, int flag);

#endif /* SIMULAZIONE_TRANSAZIONI_MSG_H */
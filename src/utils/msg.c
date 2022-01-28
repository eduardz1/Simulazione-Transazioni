#include "msg.h"

/* send a message and checks for errors */
int send_message(int queueID, void *msg, int size, int flag)
{
    msgsnd(queueID, msg, size, flag);
    switch (errno)
    {
    case EACCES:
        fprintf((FILE *)2,"[MSG SEND %d] no write permission on queue\n", getpid());
        break;
    case EAGAIN:
        fprintf((FILE *)2,"[MSG SEND %d] couldn't write on queue\n", getpid());
        break;
    case EFAULT:
        fprintf((FILE *)2,"[MSG SEND %d] address pointed by msgp inaccessible\n", getpid());
        break;
    case EIDRM:
        fprintf((FILE *)2,"[MSG SEND %d] message queue removed\n", getpid());
        break;
    case EINTR:
        TRACE(("[MSG SEND %d] signal caught when waiting for queue to free\n", getpid()))
        break;
    case EINVAL:
        fprintf((FILE *)2,"[MSG SEND %d] invalid  msqid  value,  or nonpositive mtype value, or invalid msgsz value\n", getpid());
        break;
    case ENOMEM:
        fprintf((FILE *)2,"[MSG SEND %d] system out of memory\n", getpid()); /* should basically never happen I hope */
        break;
    default:
        return SUCCESS;
        break;
    }
    return ERROR;
}

int receive_message(int queueID, void *msg, int size, int mtype, int flag)
{
    msgrcv(queueID, msg, size, mtype, flag);
    switch (errno)
    {
    case E2BIG:
        fprintf((FILE *)2,"[MSG RCV %d] the message length is greater than sizeof(transaction)\n", getpid());
        break;
    case EACCES:
        fprintf((FILE *)2,"[MSG RCV %d] no read permission on queue\n", getpid());
        break;
    case EFAULT:
        fprintf((FILE *)2,"[MSG RCV %d] address pointed by msgp inaccessible\n", getpid());
        break;
    case EIDRM:
        fprintf((FILE *)2,"[MSG RCV %d] mesage queue removed\n", getpid());
        break;
    case EINTR:
        fprintf((FILE *)2,"[MSG RCV %d] signal caught while receiving a message\n", getpid());
        break;
    case ENOMSG: /* no need to print every time */
        break;
    default:
        return SUCCESS;
        break;
    }
    return ERROR;
}
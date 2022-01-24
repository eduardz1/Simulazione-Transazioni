#include "msg.h"

int send_message(int queueID, void *msg, int size, int waitFlag)
{
    int flag = waitFlag == 0 ? 0 : 04000; /* 04000 is IPC_NOWAIT */
    msgsnd(queueID, msg, size, flag);
    switch (errno)
    {
    case EACCES:
        printf("[MSG SEND %d] no write permission on queue\n", getpid());
        break;
    case EAGAIN:
        printf("[MSG SEND %d] couldn't write on queue\n", getpid());
        break;
    case EFAULT:
        printf("[MSG SEND %d] address pointed by msgp inaccessible\n", getpid());
        break;
    case EIDRM:
        printf("[MSG SEND %d] message queue removed\n", getpid());
        break;
    case EINTR:
        TRACE(("[MSG SEND %d] signal caught when waiting for queue to free\n", getpid()));
        break;
    case EINVAL:
        printf("[MSG SEND %d] invalid  msqid  value,  or nonpositive mtype value, or invalid msgsz value\n", getpid());
        break;
    case ENOMEM:
        printf("[MSG SEND %d] system out of memory\n", getpid()); /* should basically never happen I hope */
        break;
    default:
        return SUCCESS;
    }
    return ERROR;
}
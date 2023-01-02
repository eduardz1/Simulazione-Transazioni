#include "msg.h"
#include "debug.h"
#include <errno.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

/* send a message and checks for errors */
int send_message(int queueID, const void *msg, size_t size, int flag)
{
    if (msgsnd(queueID, msg, size, flag) == 0) return EXIT_SUCCESS;
    switch (errno)
    {
    case EACCES:
        TRACE(("[MSG SEND %d] no write permission on queue\n", getpid()));
        break;
    case EAGAIN:
        TRACE(("[MSG SEND %d] couldn't write on queue\n", getpid()));
        break;
    case EFAULT:
        TRACE(
            ("[MSG SEND %d] address pointed by msgp inaccessible\n", getpid()));
        break;
    case EIDRM:
        TRACE(("[MSG SEND %d] message queue removed\n", getpid()));
        break;
    case EINTR:
        TRACE(("[MSG SEND %d] signal caught when waiting for queue to free\n",
               getpid()))
        break;
    case EINVAL:
        TRACE(("[MSG SEND %d] invalid  msqid  value,  or nonpositive mtype "
               "value, or invalid msgsz value\n",
               getpid()));
        break;
    case ENOMEM:
        TRACE(("[MSG SEND %d] system out of memory\n", getpid()));
        break;
    default: return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int receive_message(int queueID, void *msg, size_t size, long mtype, int flag)
{
    if (msgrcv(queueID, msg, size, mtype, flag) == 0) return EXIT_SUCCESS;
    switch (errno)
    {
    case E2BIG:
        TRACE(("[MSG RCV %d] the message length is greater than "
               "sizeof(transaction)\n",
               getpid()));
        break;
    case EACCES:
        TRACE(("[MSG RCV %d] no read permission on queue\n", getpid()));
        break;
    case EFAULT:
        TRACE(
            ("[MSG RCV %d] address pointed by msgp inaccessible\n", getpid()));
        break;
    case EIDRM: TRACE(("[MSG RCV %d] mesage queue removed\n", getpid())); break;
    case EINTR:
        TRACE(("[MSG RCV %d] signal caught while receiving a message\n",
               getpid()));
        break;
    case ENOMSG: /* no need to print every time */ break;
    default: return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

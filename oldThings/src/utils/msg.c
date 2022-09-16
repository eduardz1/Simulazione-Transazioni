#include "Ms_Queue.h" 

/* send a message and checks for errors */
int send_message(int queueID,void *msg,int size,int flag){
  
  if(msgsnd(queueID,msg,size,flag)==0){
    return 0; 
  }
  switch (errno)
  {
    case EACCES:
      fprintf(stderr,"[MSG SEND %d] no write permission on queue\n", getpid());
      break;
    case EAGAIN:
      fprintf(stderr,"[MSG SEND %d] couldn't write on queue\n", getpid());
      break;
    case EFAULT:
      fprintf(stderr,"[MSG SEND %d] address pointed by message inaccessible\n", getpid());
      break;
    case EIDRM:
      fprintf(stderr,"[MSG SEND %d] message queue removed\n", getpid());
      break;
    case EINTR:
      fprintf(stderr,"[MSG SEND %d] signal caught when waiting for queue to free\n", getpid());
      break;
    case EINVAL:
      fprintf(stderr,"[MSG SEND %d] invalid  msqid  value,  or nonpositive mtype value, or invalid msgsz value\n", getpid());
      break;
    case ENOMEM:
      fprintf(stderr,"[MSG SEND %d] system out of memory\n", getpid()); /* hoping this never happens */
      break;
    default:
      return SUCCESS;
      break;
  }
  return ERROR; /*error*/
}




int receive_message(int queueID,void *msg,int size,int mtype,int flag){
  if(msgrcv(queueID,msg,size,mtype,flag)==0){
    return SUCCESS; 
  }
  switch (errno)
  {
    case EACCES:
      fprintf(stderr,"[MSG SEND %d] no write permission on queue\n", getpid());
      break;
    case EAGAIN:
      fprintf(stderr,"[MSG SEND %d] couldn't write on queue\n", getpid());
      break;
    case EFAULT:
      fprintf(stderr,"[MSG SEND %d] address pointed by msgp inaccessible\n", getpid());
      break;
    case EIDRM:
      fprintf(stderr,"[MSG SEND %d] message queue removed\n", getpid());
      break;
    case EINTR:
      fprintf(stderr,"[MSG SEND %d] signal caught when waiting for queue to free\n", getpid());
      break;
    case EINVAL:
      fprintf(stderr,"[MSG SEND %d] invalid  msqid  value,  or non positive mtype value, or invalid msgs value\n", getpid());
      break;
    case ENOMEM:
      fprintf(stderr,"[MSG SEND %d] system out of memory\n", getpid()); /* hoping this never happens */
      break;
    default:
      return SUCCESS;
      break;
  }
  return ERROR;
}
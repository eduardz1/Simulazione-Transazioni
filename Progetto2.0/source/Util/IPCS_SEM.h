#include "Ms_Queue.h"




typedef struct sembuf {
   unsigned short sem_num; /* Semaphore set num */
   short sem_op; /* Semaphore operation */
   short sem_flg; /* Operation flags, IPC_NOWAIT, SEM_UNDO */
}SemBuf;


/*this union must be defined as rewuired by the semctl man page  */ 
union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
				    (Linux-specific) */
};

int sem_set_val(int sem_id, int sem_num , int sem_val ); 


int sem_released( int sem_id , int sem_num ); 

void sem_getall(char * mystring , int sem_id ) ; 

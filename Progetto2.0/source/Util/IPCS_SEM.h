/*#include "Ms_Queue.h"*/

typedef struct semphbuf
{
	unsigned short sem_num; /* Semaphore set num */
	short sem_op;						/* Semaphore operation */
	short sem_flg;					/* Operation flags, IPC_NOWAIT, SEM_UNDO */
} SemBuf;

/*this union must be defined as rewqired by the semctl man page  */
union semun
{
	int val;							 /* Value for SETVAL */
	struct semid_ds *buf;	 /* Buffer for IPC_STAT, IPC_SET */
	unsigned short *array; /* Array for GETALL, SETALL */
	struct seminfo *__buf; /* Buffer for IPC_INFO Linux-specific) */
};
/*PROTOTYPE'S FUNCTION DECLARATION */

int sem_set_val(int sem_id, int sem_num, int sem_val);
int sem_released(int sem_id, int sem_num);
int resource_set(int sem_id, int sem_count);
int resource_release(int sem_id, int sem_count);
void sem_getall(char *mystring, int sem_id);
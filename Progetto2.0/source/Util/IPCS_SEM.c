#include "IPCS_SEM.h"

/* int sem_id  = semget ( IPC_ PRIVATE  , NUM_RISORSE , 0600 )*/


int Sem_set(int sem_id, int sem_num, int sem_val)
{
	return semctl(sem_id, sem_num, SETVAL, sem_val);

}

int resource_set(int sem_id, int sem_count)
{
	struct sembuf sops = { 0 };

	sops.sem_num = sem_count;
	sops.sem_flg = 0;
	sops.sem_op = -1;
	return semop(sem_id, &sops, 1); /*system call conteined in the sem.h library check how they work visit man page   */

}

int resource_release(int sem_id, int sem_count)
{
	struct sembuf sops = { 0 };
	sops.sem_flg = 0;
	sops.sem_num = sem_count;
	sops.sem_op = -1;
	return semop(sem_id, &sops, 1);
}

void sem_print_value(char* mystring, int sem_id)
{
	union semun arg; /* man semctl per vedere def della union  */
	unsigned short* sem_vals, i;
	unsigned long num_sem;
	char cur_str[10];
	struct semid_ds my_ds = { 0 };

	/* Num of semaphore */
	arg.buf = &my_ds;
	semctl(sem_id, 0, IPC_STAT, arg);
	/*IPC_STAT =Copy information from the kernel data structure associated
				with shmid into the shmid_ds structure pointed to by buf.
				The caller must have read permission on the shared memory
				segment.
		   */
	num_sem = arg.buf->sem_nsems;

	/* Get the value of all semaphores */
	sem_vals = malloc(sizeof(*sem_vals) * num_sem);
	arg.array = sem_vals;
	semctl(sem_id, 0, GETALL, arg);
	/* GETALL MACRO =  [XSI] Return semvals into arg.array {READ} */

	/* the string myst be allocated by the caller */
	mystring[0] = 0;
	for (i = 0; i < num_sem; i++)
	{
		sprintf(cur_str, "%d", sem_vals[i]);
		/*sprintf : formatted  the string output writted in a buffer  */
		strcat(mystring, cur_str);
	}
	free(sem_vals);

}



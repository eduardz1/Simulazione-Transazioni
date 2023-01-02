#ifndef SIMULAZIONE_TRANSAZIONI_SEM_H
#define SIMULAZIONE_TRANSAZIONI_SEM_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

/* from prof. Bini examples */

/*
 * The following union must be defined as required by the semctl man
 * page
 */
union semun {
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                  (Linux-specific) */
};

/*
 * Set a semaphore to a user defined value
 * INPUT:
 * - sem_id: the ID of the semaphore IPC object
 * - sem_num: the position of the semaphore in the array
 * - sem_val: the initialization value of the semaphore
 * RESULT:
 * - the selected semaphore is initialized to the given value
 * - the returned value is the same as the invoked semctl
 */
int sem_set_val(int sem_id, int sem_num, int sem_val);

/*
 * Try to access the resource
 * INPUT:
 * - sem_id: the ID of the semaphore IPC object
 * - sem_num: the position of the semaphore in the array
 * RESULT
 * - if the resource is available (semaphore value > 0), the semaphore
 *   is decremented by one
 * - if the resource is not available (semaphore value == 0), the
 *   process is blocked until the resource becomes available again
 * - the returned value is the same as the invoked semop
 */
int sem_reserve(int sem_id, int sem_num);

/*
 * Release the resource
 * INPUT:
 * - sem_id: the ID of the semaphore IPC object
 * - sem_num: the position of the semaphore in the array
 * RESULT:
 * - the semaphore value is incremented by one. This may unblock some
 *   process
 * - the returned value is the same as the invoked semop
 */
int sem_release(int sem_id, int sem_num);

/*
 * Print all semaphore values to a string. my_string MUST be
 * previously allocated
 */
void sem_getall(char *my_string, int sem_id);

#endif /* SIMULAZIONE_TRANSAZIONI_SEM_H */

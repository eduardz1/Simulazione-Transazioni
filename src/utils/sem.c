#include "sem.h"

#ifndef TEST_ERROR
#define TEST_ERROR                                 \
    if (errno)                                     \
    {                                              \
        fprintf(stderr,                            \
                "%s:%d: PID=%5d: Error %d (%s)\n", \
                __FILE__,                          \
                __LINE__,                          \
                getpid(),                          \
                errno,                             \
                strerror(errno));                  \
    }
#endif

int errno;

/* Set a semaphore to a user defined value */
int sem_set_val(int sem_id, int sem_num, int sem_val)
{
    return semctl(sem_id, sem_num, SETVAL, sem_val);
}

/* Try to access the resource */
int sem_reserve(int sem_id, int sem_num)
{
    struct sembuf sops;

    sops.sem_num = sem_num;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    return semop(sem_id, &sops, 1);
    TEST_ERROR
}

/* Release the resource */
int sem_release(int sem_id, int sem_num)
{
    struct sembuf sops;

    sops.sem_num = sem_num;
    sops.sem_op = 1;
    sops.sem_flg = 0;

    return semop(sem_id, &sops, 1);
}

/* Print all semaphore values to a string */
int sem_getall(char *my_string, int sem_id)
{
    union semun arg; /* man semctl per vedere def della union  */
    unsigned short *sem_vals, i;
    unsigned long num_sem;
    char cur_str[10];
    struct semid_ds my_ds;

    /* Get the number of semaphores */
    arg.buf = &my_ds;
    semctl(sem_id, 0, IPC_STAT, arg);
    TEST_ERROR;
    num_sem = arg.buf->sem_nsems;

    /* Get the values of all semaphores */
    sem_vals = malloc(sizeof(*sem_vals) * num_sem);
    arg.array = sem_vals;
    semctl(sem_id, 0, GETALL, arg);

    /* Initialize the string. MUST be allocated by the caller */
    my_string[0] = 0;
    for (i = 0; i < num_sem; i++)
    {
        sprintf(cur_str, "%d ", sem_vals[i]);
        strcat(my_string, cur_str);
    }
}

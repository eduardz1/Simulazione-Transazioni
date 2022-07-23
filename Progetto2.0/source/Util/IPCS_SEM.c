#include "IPCS_SEM.h"



/* int sem_id  = semget ( IPC_ PRIVATE  , NUM_RISORSE , 0600 )*/ 


int Sem_set(int sem_id , int sem_num , int sem_val ){ 
        return semctl(sem_id,sem_num,SETVAL , sem_val); 

}
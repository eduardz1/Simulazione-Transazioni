#include "IPCS_SEM.h"



/* int sem_id  = semget ( IPC_ PRIVATE  , NUM_RISORSE , 0600 )*/ 


int Sem_set(int sem_id , int sem_num , int sem_val ){ 
        return semctl(sem_id,sem_num,SETVAL , sem_val); 

}

int resource_set(int sem_id , int sem_count) 
{ 
        SemBuf sops = {0};
        
        sops.sem_num = sem_count ; 
        sops.sem_flg =0 ; 
        sops.sem_op=-1; 
        return semop(sem_id,&sops,1); /*system call conteined in the sem.h library check how they work visit man page   */ 

}

int resouce_release(int sem_id , int sem_count)
{
        SemBuf sops ={0}; 

        sops.sem_flg = 0 ; 
        sops.sem_num = sem_count; 
        sops.sem_op = -1 ; 
        return somop(sem_id,&sops,1);
}
void sem_print_value(char *mystring , int sem_id)
{
        union semun arg ; /* man semctl per vedere def della union  */
        unsigned short * sem_vals , i ; 
        unsigned long num_sem; 
        char cur_str[10]; 

}



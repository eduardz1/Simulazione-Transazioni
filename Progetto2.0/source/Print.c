#include "include/Print.h"
#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h>

/*
 int    SO_USERS_NUM;            
 int    SO_NODES_NUM ;
 int    SO_BUDGET_INIT;
 int    SO_REWARD;
 int    SO_MIN_TRANS_GEN_NSEC;
 int    SO_MAX_TRANS_GEN_NSEC;
 int    SO_RETRY;
 int    SO_TP_SIZE;  
 int    SO_MIN_TRANS_PROC_NSEC ; 
 int    SO_MAX_TRANS_PROC_NSEC  ;
 int    SO_SIM_SEC  ;
 int    SO_FRIENDS_NUM;  
 int    SO_HOPS  ;
*/
extern char ** env_ptr; 

void *printing(void *p) {
  FILE *fp;
  fp = fopen("ledger.txt", "w+");
  fprintf(fp, "testing in file");

  fclose(fp);
  printf("--------------------------\n");
  printf("i'm *printing function\n");
  printf("-------------------------\n");
  conf_env(); 




}
/* La differenza con le global sta che ,le global vanno definite in ogni file le env no 
getenv returna una stringa */
void conf_env(){ 
    setenv("SO_USER_NUM","1000",1); 
    setenv("SO_NODES_NUM","10",1);
    setenv("SO_BUDGET_INIT","1000",1);
    setenv("SO_REWARD","20",1);
    setenv("SO_MIN_TRANS_GEN_NSEC","100000000",1);
    setenv("SO_MAX_TRANS_GEN_NSEC","100000000",1);
    setenv("SO_RETRY","2",1);
    setenv("SO_TP_SIZE","20",1);
    setenv("SO_MIN_TRANS_PROC_NSEC","1000000",1);
    setenv("SO_MAX_TRANS_PROC_NSEC","1000000",1);
    setenv("SO_SIM_SEC","10",1);
    setenv("SO_FRIENDS_NUM","2",1);
    setenv("SO_HOPS","140",1);

    char * so_user_num   = getenv("SO_USER_NUM"); 
    char * so_nodes_num  = getenv("SO_NODES_NUM");
    char * so_budget_init= getenv("SO_BUDGET_INIT");
    char * so_reward     = getenv("SO_REWARD");
    char * so_min_trans_gen_nsec = getenv("SO_MIN_TRANS_GEN_NSEC"); 
    char * so_max_trans_gen_nsec = getenv("SO_MAX_TRANS_GEN_NSEC"); 
    char * so_retry = getenv("SO_RETRY");
    char * so_tp_size = getenv("SO_TP_SIZE"); 
    char * so_min_trans_proc_nsec = getenv("SO_MIN_TRANS_PROC_NSEC"); 
    char * so_max_trans_proc_nsec = getenv("SO_MAX_TRANS_PROC_NSEC"); 
    char * so_sim_sec = getenv("SO_SIM_SEC "); 
    char * so_friends_num = getenv("SO_FRIENDS_NUM"); 
    char * so_hops = getenv("SO_HOPS"); 

    /*Set value for env varible */
    
    /*Print env variable  */
    printf("SO_USER_NUM:%s\n",so_user_num);
    printf("SO_NODES_NUM:%s\n",so_nodes_num); 
    printf("SO_BUDGET_INIT:%s\n",so_budget_init);
    printf("SO_REWARD:%s\n",so_reward); 
    printf("SO_MIN_TRANS_GEN_NSEC:%s\n",so_min_trans_gen_nsec);
    printf("SO_MAX_TRANS_GEN_NSEC:%s\n",so_max_trans_gen_nsec); 
    printf("SO_RETRY:%s\n",so_retry); 
    printf("SO_TP_SIZE:%s\n",so_tp_size);
    printf("SO_MIN_TRANS_PROC_NSEC:%s\n",so_min_trans_proc_nsec); 
    printf("SO_MAX_TRANS_PROC_NSEC:%s\n",so_max_trans_proc_nsec); 
    printf("SO_SIM_SEC:%s\n",so_sim_sec); 
    printf("SO_FRIENDS_NUM:%s\n",so_friends_num); 
    printf("SO_HOPS:%s\n",so_hops); 


exit(EXIT_SUCCESS); 

}



/* 
void  conf_value() {
  SO_USERS_NUM = 6; 
  SO_NODES_NUM = 10 ; 
  SO_BUDGET_INIT = 1000; 
  SO_REWARD = 20 ;
  SO_MIN_TRANS_GEN_NSEC =100000000;
  SO_MAX_TRANS_GEN_NSEC =100000000;
  SO_RETRY = 2 ; 
  SO_TP_SIZE = 20 ; 
  SO_MIN_TRANS_PROC_NSEC =1000000;
  SO_MAX_TRANS_PROC_NSEC =1000000;
  SO_SIM_SEC = 10 ;
  SO_FRIENDS_NUM = 3;
  SO_HOPS = 140 ; 

  printf("SO_USER_NUM=%d\n", SO_USERS_NUM);
  printf("SO_NODES_NUM=%d\n", SO_NODES_NUM); 
  printf("SO_BUDGET_INIT=%d\n",SO_BUDGET_INIT);
  printf("SO_REWARD =:%d\n",SO_REWARD);
  printf("SO_MIN_TRANS_GEN_NSEC=%d\n",SO_MIN_TRANS_GEN_NSEC);
  printf("SO_MAX_TRANS_GEN_NSEC=%d\n",SO_MAX_TRANS_GEN_NSEC);
  printf("SO_RETRY=:%d\n",SO_RETRY);
  printf("SO_TP_SIZE=%d\n",SO_TP_SIZE);
  printf("SO_MIN_TRANS_PROC_NSEC=%d\n",SO_MIN_TRANS_PROC_NSEC);
  printf("SO_SIM_SEC=%d\n",SO_SIM_SEC);
  printf("SO_FRIENDS_NUM=%d\n",SO_FRIENDS_NUM);
  printf("SO_HOPS=%d\n",SO_HOPS);
  exit(EXIT_SUCCESS);
  
}

*/ 

/*thread initialization according to
 * https://www.includehelp.com/articles/threading-in-c-programming-language-with-gcc-linux.aspx
 */
int main(){
  pthread_t threadId;
  int P_Parent;
  int thTest;
  int *ptr;
  P_Parent = 1;
  thTest = pthread_create(&threadId, NULL, printing(0),P_Parent);
  if (thTest == 0) {
    printf("thread work without error :)\n");
    pthread_join(threadId, (void **)&ptr);
    printf("%i\n", *ptr);

  } else {
    perror("thread doesn't work  :(\n");
    exit(EXIT_FAILURE); 
  }
  printf("printing from main\n");
}

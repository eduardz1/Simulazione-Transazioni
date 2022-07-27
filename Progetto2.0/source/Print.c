#include "include/Print.h"
#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h>


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



void *printing(void *p) {
  FILE *fp;
  fp = fopen("ledger.txt", "w+");
  fprintf(fp, "testing in file");

  fclose(fp);
  printf("--------------------------\n");
  printf("i'm *printing function\n");
  printf("-------------------------\n");
  conf_value(); 




}


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



/*thread initialization according to
 * https://www.includehelp.com/articles/threading-in-c-programming-language-with-gcc-linux.aspx
 */
int main() {
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

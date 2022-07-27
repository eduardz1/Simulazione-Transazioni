#include "include/Print.h"
#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h>


void *printing(void *p) {
  FILE *fp;
  fp = fopen("ledger.txt", "w+");
  fprintf(fp, "testing in file");

  fclose(fp);
  printf("--------------------------\n");
  printf("i'm *printing function\n");
  printf("-------------------------\n");
  CONF(); 



/*
  printf("----------- Configuration value ------------\n");
  printf("SO_USER_NUM->%u\n", par->SO_USER_NUM);
  printf("SO_NODES_NUM->%u\n", par->SO_NODES_NUM);
  printf("SO_BUDGET_INIT->%u\n", par->SO_BUDGET_INIT);
  printf("SO_REWARD->%c\n", par->SO_REWARD);
  printf("SO_MIN_TRANS_GEN_NSEC->%lu\n", par->SO_MIN_TRANS_GEN_NSEC);
  printf("SO_MAX_TRANS_GEN_NSEC->%lu\n", par->SO_MAX_TRANS_GEN_NSEC);
  printf("SO_RETRY->%u\n", par->SO_RETRY);
  printf("SO_TP_SIZE->%u\n", par->SO_TP_SIZE);
  printf("SO_MIN_TRANS_PROC_NSEC->%lu\n", par->SO_MIN_TRANS_PROC_NSEC);
  printf("SO_MAX_TRANS_PROC_NSEC->%lu\n", par->SO_MAX_TRANS_PROC_NSEC);
  printf("SO_SIM_SEC->%u\n", par->SO_SIM_SEC);
  printf("SO_FRIENDS_NUM->%u\n", par->SO_FRIENDS_NUM);
  printf("SO_HOPS->%u\n", par->SO_HOPS);
  */
}


int CONF() {
  putenv("SO_USER_NUM=100");
  printf("SO_USER_NUM:%d\n", atoi(getenv("SO_USER_NUM")));
  putenv("SO_NODES_NUM=10");
  printf("ENV:%d\n", atoi(getenv("SO_NODES_NUM")));
  putenv("SO_FRIENDS_NUM=5");
  printf("ENV:%d\n", atoi(getenv("SO_FRIENDS_NUM")));
  putenv("SO_BUDGET_INIT=1000");
  printf("ENV:%d\n", atoi(getenv("SO_BUDGET_INIT")));
  putenv("SO_REWARD=20");
  printf("ENV:%d\n", atoi(getenv("SO_REWARD")));
  putenv("SO_MIN_TRANS_GEN_NSEC=10000000");
  printf("ENV:%d\n", atoi(getenv("SO_MIN_TRANS_GEN_NSEC")));
  putenv("SO_MAX_TRANS_GEN_NSEC=200000000");
  printf("ENV:%d\n", atoi(getenv("SO_MAX_TRANS_GEN_NSEC")));
  putenv("SO_RETRY=2");
  printf("ENV:%d\n", atoi(getenv("SO_RETRY")));
  putenv("SO_TP_SIZE=20");
  printf("ENV:%d\n", atoi(getenv("SO_TP_SIZE")));
  putenv("SO_MIN_TRANS_PROC_NSEC=1000000 ");
  printf("ENV:%d\n", atoi(getenv("SO_MIN_TRANS_PROC_NSEC")));
  putenv("SO_MAX_TRANS_PROC_NSEC=1000000");
  printf("ENV:%d\n", atoi(getenv("SO_MAX_TRANS_PROC_NSEC")));
  putenv("SO_REWARD=20");
  printf("ENV:%d\n", atoi(getenv("SO_REWARD")));
  putenv("SO_SIM_SEC=10");
  printf("ENV:%d\n",atoi(getenv("SO_SIM_SEC")));
  putenv("SO_HOPS = 140"); 
  printf("ENV:%d\n", atoi(getenv("SO_HOPS"))); 
  exit(EXIT_SUCCESS);
  return 0;
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

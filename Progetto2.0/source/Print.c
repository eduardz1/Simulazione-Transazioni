#include "include/Print.h"
#include "include/Conf.h"
#include <pthread.h> 


struct ConfigParameters *par;

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
    printf("thread doesn't work  :(\n");

    return 0;
  }
  printf("printing from main\n");
}

#include "include/Common.h"


void *printing(FILE *fp,struct ConfigParameters *par){
/*printing parameters from config */
	printf("--------------------------------------------\n----------- Configuration value ------------\n");
    printf("SO_USER_NUM->%u\n", par->SO_USER_NUM);
    printf("SO_NODES_NUM->%u\n", par->SO_NODES_NUM);
    printf("SO_BUDGET_INIT->%u\n", par->SO_BUDGET_INIT);
    printf("SO_REWARD->%u\n", par->SO_REWARD);
    printf("SO_MIN_TRANS_GEN_NSEC->%lu\n", par->SO_MIN_TRANS_GEN_NSEC);
    printf("SO_MAX_TRANS_GEN_NSEC->%lu\n", par->SO_MAX_TRANS_GEN_NSEC);
    printf("SO_RETRY->%u\n", par->SO_RETRY);
    /*printf("SO_TP_SIZE->%u\n", par->SO_TP_SIZE); */ 
    printf("SO_MIN_TRANS_PROC_NSEC->%lu\n", par->SO_MIN_TRANS_PROC_NSEC);
    printf("SO_MAX_TRANS_PROC_NSEC->%lu\n", par->SO_MAX_TRANS_PROC_NSEC);
    printf("SO_SIM_SEC->%u\n", par->SO_SIM_SEC);
    printf("SO_FRIENDS_NUM->%u\n", par->SO_FRIENDS_NUM);
    printf("SO_HOPS->%u\n", par->SO_HOPS);
    printf("--------------------------------------------\n");









	fprintf(fp,"i'm a thread\n");

}

/*thread initialization according to https://www.geeksforgeeks.org/multithreading-c-2/ */
int main(){
pthread_t threadId;
pthread_create(&threadId,NULL,printing,NULL);
pthread_join(threadId,NULL);

FILE *fp;
fp=fopen("ledger.txt","w+");
fprintf(fp,"i'm out of thread\n");
fclose(fp);
}

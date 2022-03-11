#include <pthread.h>
#include "include/Common.h"
struct ConfigParameters *par;
void *printing(){

printf("printing from thread");
FILE *fp;
	fp=fopen("ledger.txt","w+");
	fprintf(fp,"print in file testing");


	
	
	fclose(fp);


	
}

/*thread initialization according to https://www.includehelp.com/articles/threading-in-c-programming-language-with-gcc-linux.aspx */
int main(){
pthread_t threadId;
int thTest;
thTest=pthread_create(&threadId,NULL,printing,NULL);
pthread_join(threadId,NULL);  /* https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-pthread-join-wait-thread-end*/
if(thTest==0){
    printf("thread is working :)\n");
} else {
    printf("thread not working :(");
    return 0;
}
printf("i'm out of thread\n");
printf("--------------------------------------------\n----------- Configuration value ------------\n");
    printf("SO_USER_NUM->%u\n", par->SO_USER_NUM);
    printf("SO_NODES_NUM->%u\n", par->SO_NODES_NUM);
    printf("SO_BUDGET_INIT->%u\n", par->SO_BUDGET_INIT);
    printf("SO_REWARD->%u\n", par->SO_REWARD);
    printf("SO_MIN_TRANS_GEN_NSEC->%lu\n", par->SO_MIN_TRANS_GEN_NSEC);
    printf("SO_MAX_TRANS_GEN_NSEC->%lu\n", par->SO_MAX_TRANS_GEN_NSEC);
    /*printf("SO_TP_SIZE->%u\n",par->SO_TP_SIZE); not working need to understand why*/
    printf("SO_MIN_TRANS_PROC_NSEC->%lu\n", par->SO_MIN_TRANS_PROC_NSEC);
    printf("SO_MAX_TRANS_PROC_NSEC->%lu\n", par->SO_MAX_TRANS_PROC_NSEC);
    printf("SO_SIM_SEC->%u\n", par->SO_SIM_SEC);
    printf("SO_FRIENDS_NUM->%u\n", par->SO_FRIENDS_NUM);
    printf("SO_HOPS->%u\n", par->SO_HOPS);

}

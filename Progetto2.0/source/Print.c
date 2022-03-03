#include <pthread.h>
#include "include/Common.h"
void *printing(){
	printf("i'm a thread\n");
}

/*thread initialization according to https://www.geeksforgeeks.org/multithreading-c-2/ */
int main(){
pthread_t threadId;
pthread_create(&threadId,NULL,printing,NULL);
pthread_join(threadId,NULL);
printf("i'm out of thread\n");
}

#include <pthread.h>
#include "include/Common.h"
void *printing(){
<<<<<<< HEAD
	printf("i'm in thread");
=======
	printf("i'm a thread\n");
>>>>>>> 57362c96dddc16d5fc2d402aa9756e66f69e6722
}

/*thread initialization according to https://www.geeksforgeeks.org/multithreading-c-2/ */
int main(){
pthread_t threadId;
pthread_create(&threadId,NULL,printing,NULL);
pthread_join(threadId,NULL);
printf("i'm out of thread\n");
}

#include <pthread.h>
void *printing(){

}

/*thread initialization according to https://www.geeksforgeeks.org/multithreading-c-2/*/
int main(){
pthread_t threadId;
pthread_create(&threadId,NULL,printing,NULL);
pthread_join(threadId,NULL);
}
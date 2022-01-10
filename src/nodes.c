#include <time.h>
#include "include/common.h"

int main(int argc, char *argv[]){
    int myPID = getpid();
    printf("Node %d has finished\n", myPID);
    return;
}
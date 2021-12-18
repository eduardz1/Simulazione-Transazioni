#define _GNU_SOURCE

#include <stdio.h>
#include <time.h>


/*
  This is the correct implementation to get a precise timestamp
 for the transactions, lesson 2021_12_13 SO T4, prof Druetto
*/

int tracktime(){
    struct timespec tp; // need to divide seconds and nanoseconds because it wouldn't fit in a long otherwise

    clock_gettime(CLOCK_MONOTONIC, &tp);
    printf("Monotonic time: %10ld.%10ld\n", tp.tv_sec, tp.tv_nsec);

    clock_gettime(CLOCK_REALTIME, &tp);
    printf("Real time: %10ld.%10ld\n", tp.tv_sec, tp.tv_nsec);

  return 0;
}
/*
typedef struct timestamp {
    long long CLOCK_REALTIME;
}timestamp;
*/
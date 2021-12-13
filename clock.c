#define _GNU_SOURCE

#include <stdio.h>
#include <time.h>

int main()
{
    struct timespec tp; // need to divide seconds and nanoseconds because it wouldnt fit in a long otherwise

    clock_gettime(CLOCK_MONOTONIC, &tp);
    printf("Monotonic time: %10ld.%10ld\n", tp.tv_sec, tp.tv_nsec);

    clock_gettime(CLOCK_REALTIME, &tp);
    printf("Real time: %10ld.%10ld\n", tp.tv_sec, tp.tv_nsec);

    return 0;
}
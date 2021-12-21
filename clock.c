/* 
 * This is the correct implementation to get a precise timestamp 
 * for the transactions, lesson 2021_12_13 SO T4, prof Druetto 
 */
struct timeStamp()
{
    struct timespec tp; /* need to divide seconds and nanoseconds because it wouldnt fit in a long otherwise */

    clock_gettime(CLOCK_REALTIME, &tp);
    /* printf("Real time: %10ld.%10ld\n", tp.tv_sec, tp.tv_nsec); */

    return tp;
}

#include <stdio.h>
#include <time.h>

void formatted_timestamp();


int main(int argc, char const *argv[])
{
    formatted_timestamp(); 
    return 0;
}



void formatted_timestamp(){
    printf("Inizio\n");
    clock_t tic = clock();
    clock_t start = clock();
    clock_t stop = clock();

    time_t rawtime;
    time_t now;
    struct tm *info;
    struct tm *today;
    double elapsed;
    char buf[128];

    time(&now);
    today = localtime(&now);
    strftime(buf, 128, "%Y/%m/%d", today);
    printf("%s\n", buf);

    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC; /* time ./a.out*/
}

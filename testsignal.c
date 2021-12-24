#include <signal.h>
#include <stdio.h>
#include <stdlib.h>   // to have funcion exit
#include <unistd.h> //to have funcion  pause
void crtlchandler(int); //include in .h
int main();

int main(){
    printf("inizio\n");

    signal(SIGINT, crtlchandler);
    while(1){
        pause();
        return 0;
    }

}

void crtlchandler(int sig){
    char c;
    signal(sig, SIG_IGN);

    printf("se hai premuto ctrl-c stampa questa frase");

    c=getchar();

}

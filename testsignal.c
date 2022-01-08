#include <signal.h>
#include <stdio.h>
#include <stdlib.h>   // to have funcion exit
#include <unistd.h> //to have funcion  pause
void crtlchandler(int); //include in .h
int main();
void exit();

int main(){
    printf("inizio\n");

    signal(SIGINT, crtlchandler);
    while(1){

    }

}


void crtlchandler(int sig){
    char c;
    signal(sig, SIG_IGN);

    printf("se hai premuto ctrl-c stampa questa frase");

    c=getchar();

    exit(0);
<<<<<<< HEAD
}
=======

}

>>>>>>> 91cac5e173845229e25a1c647384bf5f20fcf43e

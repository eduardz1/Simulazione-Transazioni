#define _GNU_SOURCE 
#include <stdio.h> 
#include <sys/types.h> 
#include <stdlib.h>
#include <signal.h>  
#include <unistd.h>


/* HANDLER IN TEORIA FUNZIONANTE MA DA IMPLEMENTARE A 
-USER 
-NODE
-MASTER 
OGNI PROCESSO AVRA' IL SUO HANDLER DEI SIGNALI 
DA CAPIRE SE USARE UNA SIGACTION O UN HANDLER CON SWITCH CASE O CON ALTRO MODO 
*/
void signal_handler_p(int signum); 
void signal_handler_c(int signum);	
	
	
	
void signal_handler_p(int signum){ 

	printf("PARENT: RECIVE SIGNAL BUT I CAN T DO ANYTHINK\n"); 
  } 


void signal_handler_c(int signum){ 
	printf("CHILD: RECIVED SIGNAL OK DAD I M GOING TO THE BAD\n"); 
	sleep(1);
	kill(getpid(),SIGUSR1);
 } 	
 
 int main(){
 pid_t pid;
  if((pid=fork())<0){
    printf("Fork Failed\n");
    exit(1);
  }
  /* Child Process */
  else if(pid==0){
    signal(SIGUSR1,signal_handler_c); // Register signal handler
    printf("Child: waiting for signal\n");
    pause();
  }
  /* Parent Process */
  else{
    signal(SIGUSR1,signal_handler_p); // Register signal handler
    sleep(1);
    printf("Parent: sending signal to Child\n");
    kill(pid,SIGUSR1);
    printf("Parent: waiting for response\n");
    pause();
  }
  return 0;
  }
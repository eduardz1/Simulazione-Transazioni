#include "struct.h"
#define TRUE 1 



int main(int argc, char const *argv[])
{
    /* code */



	FILE *fp;
	int id;
	int i;
	struct parametri par; 
	key_t key = ftok("key.txt",'100'); 
	//key_t key = KEY;
	//fp=fopen("input.txt", "r"); 
	/*if(fp ==NULL){ 
	perror("ERROR IN FILE\n");
	exit(1); 
	}*/ 
	msg context; 
	context.mytpe=1;
	id=msgget(key , IPC_CREAT|0666);
	//context.Mypid=getpid(); 
	//context.reward = 4422; 
    int num; 
	num = 10; 
    //context.mtext[100] = num; //sprintf(context.mtext, "%d", num); 
	//fgets(context.mesText,100,stdin);
	//fclose(fp); 

	par.bool = 1; 
	par.myPid=getpid(); 
	par.reward=100; 

	context.pid = getpid(); 
	context.reward = 100; 
	context.bool = TRUE; 
	printf("SENDING MESSAGE[ID_MSG:%d]\n",id);
	msgsnd(id, (void*)&context,sizeof(context),0 ); 
	printf("[ID %d] MESSAGE SEND IS %d\n %d\n %d",id,context.pid, context.reward,context.bool);
	msgctl(id,IPC_STAT,&par); 
    return 0 ; 
} 
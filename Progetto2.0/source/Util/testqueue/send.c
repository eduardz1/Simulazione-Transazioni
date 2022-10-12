
#include "key.h"



typedef struct msgbuf_trans
{ 
  long m_type;
  char mesText[100];
}Message;

int main (){ 
	
	FILE *fp;
	int id;
	key_t key = ftok("key.txt",'100'); 
	//key_t key = KEY;
	fp=fopen("input.txt", "r"); 
	if(fp ==NULL){ 
	perror("ERROR IN FILE\n");
	exit(1); 
	}
	Message context; 
	context.m_type=1;
	id=msgget(key , IPC_CREAT|0666);
	fgets(context.mesText , 100 ,fp); 
	//fgets(context.mesText,100,stdin);
	printf("This is file context:\n %s",context.mesText); 
	fclose(fp); 
	printf("SENDING MESSAGE[ID_MSG:%d]\n",id);
	msgsnd(id, &context,sizeof(long)+sizeof(char)*100,0);
	printf("[SENDER:%d]TEXT SENDED:%c",id,context.mesText);
return 0;

} 
#include "key.h"


typedef struct msgbuf_trans
{ 
  long m_type;
  char mesText[100];
}Message;


int main(){ 
	Message rcv; 
	key_t key = ftok("key.txt",'100'); 
	//key_t key =KEY;
	int id; 
	id= msgget(key,IPC_CREAT|0666);
	printf("[RECIVE %d]WAITING MESSAGE\n",id); 
	msgrcv(id,&rcv ,sizeof(rcv),1,0);
	printf("[RECIVE: PID]:THIS IS THE MESSAGE :%s\n" ,rcv.mesText);
return 0;

} 
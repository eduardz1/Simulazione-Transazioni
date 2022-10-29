 
 #include "struct.h"
 
 int main(int argc, char const *argv[])
 {  
	
	msg rcv; 
	msg context;; 
	key_t key = ftok("key.txt",'100'); 
	//key_t key =KEY;
	int id; 
	int i; 
	id= msgget(key,0666|IPC_CREAT);
	printf("[RECIVE %d]WAITING MESSAGE\n",id); 
	msgrcv(id,(void*)&rcv ,sizeof(rcv),1,0);

	printf("[ID %d\n] MESSAGE SEND IS\n %d\n %d\n %d",id,rcv.pid, rcv.reward,rcv.bool);

	//printf("[RECIVE: PID %d]:THIS IS THE MESSAGE :%d\n" ,getpid(),rcv.mtext);
	//printf("[RECIVE: PID %d]:THIS IS THE MESSAGE :%d\n" ,id,rcv.mtext[2]);
	
	
	return 0;
 }
 
  
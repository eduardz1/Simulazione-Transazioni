#include "test_msqueue.h"


/*ISTANCE A MS_QUEUE WITH MSGGET FUCNTION */

int msg_queue(pid_t key,int msgflg){
    if (key <= 0 ){
        dprintf(2,"[MSG QUEUE %d] invalid key\n",getpid());
        return PROBLEM;
        }
    {
       msgget( key, msgflg); 
   }
    return key; 
}

/*send a message */
int msg_send(pid_t key  , struct msgbuf *msg , size_t size , int msgflg ){ 
 if(msgsnd(key,msg, size ,msgflg <= 0 )){
    dprintf(2,"[MSG SEND %d] couldn't write on queue\n", getpid());
    return PROBLEM;
    }
    msgsnd(key,msg, size ,msgflg);
    return key;
 }
     
/*receive a message */
int msg_receive(pid_t key , struct msgbuf *msg , size_t size , long mtype , int msgflg ){
    if(msgrcv(key,msg,size,mtype,msgflg <= 0 )){
        dprintf(2,"[MSG RECEIVE %d] couldn't write on queue\n", getpid());
        return PROBLEM;
    }
    msgrcv(key,msg,size,mtype,msgflg);
    return key;
}


/*RAGGIONAMENTO --> Dopo avere istanziato queste tre funzioni per la messaggistica ora capiamo 

------------------------------------
il percorso che fara' il messaggio 
1) transazione con tutti gli attributi viene prelavata dagli user 
2) spedita nel nodo cioe' ,pertanto nella linked list 
3) transazione elaborata con un status finale 
4) spedita nel ladger dove viene registarta 

--------------------------


Ora che si dovrebbe gestire nel main la situazione narrata sopra 
*/ 


int main(int argc, char const *argv[])
{
    
    return 0;
}

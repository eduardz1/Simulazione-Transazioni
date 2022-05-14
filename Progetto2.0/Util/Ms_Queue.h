#include "/Users/popper/Desktop/Folders/RiProgetto/Simulazione-Transazioni/Progetto2.0/source/include/Common.h"
key_t Mes_Key ; 
int Message_ID; 
int Message_get; 
int Message_Send; 
int Message_Rcv; 
int Message_Ctrl; 
const void *Message_p; 
size_t Message_size; 
struct msgbuf_trans * Message_Buff; 



#define MESSAGE_KEY 0x21


struct msgbuf_trans
{
  long m_type;
  char mesText[1];
    struct message
    {
      int hops; 
      transaction uTrans;
      struct msgbuf_trans *next;
    }Message_Transaction;
};






void Message_Queue(){ 
    
    
    Message_get= msgget(Mes_Key, IPC_CREAT); /*IF FALILED RETURN -1 */
    Message_Send=msgsnd(Message_ID,Message_p,Message_size,IPC_NOWAIT); /*IPC_NOWAIT return  "1" if Failed*/
    Message_Rcv= msgrcv(Message_ID,&Message_p,Message_size,Message_Buff->m_type,IPC_NOWAIT);
    Message_Ctrl=msgctl(Message_ID,IPC_RMID,NULL);

    /*PRINT'S */
    printf("MESS_RECIVED",Message_Buff->mesText);    
    system("MESG_out.txt"); 
     
}
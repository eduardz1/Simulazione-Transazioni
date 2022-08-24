

const void *Message_p; 
size_t Message_size; 
 





typedef struct msgbuf_trans
{ 
  size_t Size_Msg; 
  int  m_type;
  char mesText[1];
    struct message
    {
      int hops; 
      transaction uTrans;
      struct msgbuf_trans *next;
    }Message_Transaction;
}Message;

/*PROTOTYPE'S */
void Message_Queue();

int send_message(int queueID,void *msg,int size,int flag);
int receive_message(int queueID,void *msg,int size,int mtype,int flag);

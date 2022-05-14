#include "../include/Common.h"
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

void Message_Queue();


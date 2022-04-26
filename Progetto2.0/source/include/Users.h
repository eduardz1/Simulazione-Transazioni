#include "Common.h";
struct node {
  transaction trans;
  struct node *next;
};


void CurrentBalance(); 
void Sh_MemUser(key_t key ,size_t size,int shmflg);

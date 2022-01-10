#include <stdio.h>
<<<<<<< HEAD
#include <sys/types.h>

#define SO_NODES_NUM
typedef struct node{
    struct timestamp timeStamp;
    pid_t sender;
    pid_t receiver;
=======
#include <time.h>
#define SO_NODES_NUM
typedef struct node{
    struct timestamp timeStamp;  /*print in the list */
    pid_t sender;   /*print in the list */
    pid_t receiver; /*print in the list */
>>>>>>> 91cac5e173845229e25a1c647384bf5f20fcf43e
    int quantity;
    int reward;

    struct node *node_next;
};
typedef struct block {
    SO_NODES_NUM;

    struct node **block;
    struct block **node_block;
};

struct node *first=NULL;
struct block **first=NULL;
void printLists(){
    struct node *first=first;
        while(first!=NULL){
            printf("%d,%d",first->timeStamp, first->sender, first->receiver);
    }
	
}

<<<<<<< HEAD
int main(){
/* liste di prova(ancora da mettere)*/
=======
void search_Timestamp(){

    

}
void search_Sender(){


} 

void search_Receiver(){

>>>>>>> 91cac5e173845229e25a1c647384bf5f20fcf43e
}


int main(){
/* liste di prova*/
}
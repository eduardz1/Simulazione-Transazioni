#include <stdio.h>
#include <time.h>
#define SO_NODES_NUM
typedef struct node{
    struct timestamp timeStamp;  /*print in the list */
    pid_t sender;   /*print in the list */
    pid_t receiver; /*print in the list */
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

void printLists(){
    struct node *first=first;
        while(first!=NULL){
            printf("%d,%d",first->timeStamp, first->sender, first->receiver);
    }
}

void search_Timestamp(){

    

}
void search_Sender(){


} 

void search_Receiver(){

}


int main(){
/* liste di prova*/
}
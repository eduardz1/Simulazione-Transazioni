#include <stdio.h>
#include <sys/types.h>

#define SO_NODES_NUM
typedef struct node{
    struct timestamp timeStamp;
    pid_t sender;
    pid_t receiver;
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
struct block **first=NULL!;
void printLists(){
    struct node *first=first;
        while(first!=NULL){
            printf("%d,%d",first->timestamp, first->sender, first->receiver);
    }
	
}

int main(){
/* liste di prova(ancora da mettere)*/
}



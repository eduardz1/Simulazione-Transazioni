/*libro mastro ----> linked list */

#include "clock.h"
#include <stdlib.h> 
#include <sys/types.h>
#define SO_REGISTRY_SIZE	/*max lenght of consecutive block */
#define SO_BLOCK_SIZE		/*number of transaction per block */



typedef struct node {            /*add explanation */
	timestamp timestamp;
	pid_t sender;
	pid_t receiver;
	int quantity;
	int reward;

	struct node *next_node;
} node;
struct node *head=NULL;
//struct node *current = NULL;



typedef struct block { 
	SO_BLOCK_SIZE;

	struct node  **block;
	struct block  **next_block;
};

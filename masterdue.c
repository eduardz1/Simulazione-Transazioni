/* libro mastro ----> linked list */

/*
 * execve(const char *user, char *const argv[], char *const envp[])
 * in user_fork to link "user" executable to the forked process, same thing for nodes
 * check lesson on pipes by prof. bini, the second hour
 */

/*
 * get every user_pid, ask to libro_mastro
 * to return it's current_budget and print
 * repeat every second until simulation persists
 * remember that CTRL-C should kill the simulation
 */

/*
 * need to define a kill signal for the simulation, either:
 * - SO_SIM_SEC seconds have passed
 * - libro_mastro is full
 * - CTRL-C from stdin
 */

/*
 * end of simulation should print:
 * - kill signal
 * - balance of every user, as before, may need to write a funciton for that
 * - balance of every node (function as before but with different parameter)
 * - number of user processes aborted
 * - number of blocks in the libro_mastro
 * - number of transaction still in the pool, for each node
 */

#include "clock.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#define SO_REGISTRY_SIZE     /* max length of consecutive block */
#define SO_BLOCK_SIZE        /* number of transaction per block*/
/*
 * #define SO_USERS_NUM
 * #define SO_NODES_NUM
 */
#define SO_NUM_FRIENDS

typedef struct node {
    timestamp timeStamp;
    pid_t sender;
    pid_t receiver;
    int quantity;
    int reward;

    struct node *next_node;
} node;

struct node *head = NULL;
struct node *current = NULL;


typedef struct block {
    SO_NODES_NUM;

    struct node **block;
    struct block **next_block;
} block;

int main() {
    struct sigaction sa_ctrl
    -c; /* we need to define an handler for CTRL-C command that closes any IPC object */
    sa_ctrl - c.sa_handler = user_transactions_handle;
    sigaction(SIGINT, &sa, NULL);

    int Prop1 = fork(SO_NODES_NUM);
    int Prop2 = fork(SO_USERS_NUM);
    switch (createdPidBlock[SO_NODES_NUM] = fork(SO_NODES_NUM)) {
        case -1: /* Case Error */
            if (Prop1 && Prop2 == 0) {
                printf("Fork error");
                exit(1);
            }
            break;
        case 0: /* Case Child */
            if (Prop1 && Prop2 > 0) {
                /*	printf("Child pid of SO_NODES_NUM is:%d\n", getpid(),Prop1);
                     printf("Child pid of SO_USERS_NUM is%d\n", getpid(),Prop2); */
            }

            break;

        default: /* Case Parent */
            fork(SO_NODES_NUM);

            fork(SO_USERS_NUM);
            break;
    }


}

void interrupt_handle(int SIGINT) {
    /*
     * remove any IPC object we created then terminate the process
     */
}

int printPid() {
    int i = 0;
    int k = 0;
    for (int i; i < SO_NODES_NUM; i++) {
        printf("generated SO_NODES pid\n %d\n", getpid());
    }
    for (int k; k < SO_USERS_NUM; k++) {
        printf("generated SO_USERS pid\n %d\n", getpid());
    }
}
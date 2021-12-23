#include "master.h"

int main() {
    struct node *head = NULL;
    struct node *current = NULL;

    struct sigaction sa_ctrl-c; /* we need to define an handler for CTRL-C command that closes any IPC object */
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
                    /*
                     * printf("Child pid of SO_NODES_NUM is:%d\n", getpid(),Prop1);
                     * printf("Child pid of SO_USERS_NUM is%d\n", getpid(),Prop2);
                     */
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
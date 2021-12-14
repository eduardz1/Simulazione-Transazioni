#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#define _GNU_SOURCE
#define SO_USERS_NUM
#define SO_NODES_NUM
#define SO_NUM_FRIENDS
#define SO_SIM_SEC

int main()

{
    int Prop1 = fork(SO_NODES_NUM);
    int Prop2 = fork(SO_USERS_NUM);
    switch (fork())
    {
    case -1: //Case Error
        if (Prop1 && Prop2 == 0)
        {
            printf("Fork error");
            exit(1);
        }
        break;
    case 0: // Case Child
        if (Prop1 && Prop2 > 0)
        {
            //printf("Child pid of SO_NODES_NUM is:%d\n", getpid(),Prop1);
            //printf("Child pid of SO_USERS_NUM is%d\n", getpid(),Prop2);
        }

        break;

    default: //Case Parent
        fork(SO_NODES_NUM);
          
        fork(SO_USERS_NUM);
        break;
    }
}

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

#define SO_USERS_NUM
#define SO_NODES_NUM
int main()
{
    switch (fork())
    {
    case -1://Case Error
       
    
        break;
    case 0: // Case Child

        break;

    default: //Case Parent

        break;
    }
}
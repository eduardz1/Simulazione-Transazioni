#include <stdlib.h>
#include <stdio.h>
#include "include/Common.h"
struct ConfigParameters *par;
int main (int argc, char *argv[])
{
  /*unsigned int SO_USER_NUM;*/
  putenv("SO_USER_NUM=100");
  printf("ENV:%d\n",atoi(getenv("SO_USER_NUM")));
  return 0;
}


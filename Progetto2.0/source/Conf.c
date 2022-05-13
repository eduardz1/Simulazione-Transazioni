/* THIS FILE CONTAIN THE VALUE OF THE CONFIGURATION PARAMETERS*/
int CONF() {
  putenv("SO_USER_NUM=100");
  printf("ENV:%d\n", atoi(getenv("SO_USER_NUM")));
  putenv("SO_NODES_NUM=10");
  printf("ENV:%d\n", atoi(getenv("SO_NODES_NUM")));
  putenv("SO_FRIENDS_NUM=5");
  printf("ENV:%d\n", atoi(getenv("SO_FRIENDS_NUM")));
  putenv("SO_BUDGET_INIT=1000");
  printf("ENV:%d\n", atoi(getenv("SO_BUDGET_INIT")));
  putenv("SO_REWARD=20");
  printf("ENV:%d\n", atoi(getenv("SO_REWARD")));
  putenv("SO_MIN_TRANS_GEN_NSEC=10000000");
  printf("ENV:%d\n", atoi(getenv("SO_MIN_TRANS_GEN_NSEC")));
  putenv("SO_MAX_TRANS_GEN_NSEC=200000000");
  printf("ENV:%d\n", atoi(getenv("SO_MAX_TRANS_GEN_NSEC")));
  putenv("SO_RETRY=2");
  printf("ENV:%d\n", atoi(getenv("SO_RETRY")));
  putenv("SO_TP_SIZE=20");
  printf("ENV:%d\n", atoi(getenv("SO_TP_SIZE")));
  putenv("SO_MIN_TRANS_PROC_NSEC");
  printf("ENV:%d\n", atoi(getenv("SO_MIN_TRANS_PROC_NSEC")));
  putenv("SO_MAX_TRANS_PROC_NSEC");

  return 0;
}

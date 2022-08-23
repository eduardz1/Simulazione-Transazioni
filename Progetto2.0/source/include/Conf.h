#include <stdio.h> 
#include <stdlib.h>
#define SO_USERS_NUM            atoi(getenv("SO_USERS_NUM"))
#define SO_NODES_NUM            atoi(getenv("SO_NODES_NUM "))
#define SO_REWARD               atoi(getenv("SO_REWARD"))
#define SO_RETRY                atoi(getenv("SO_RETRY"))
#define SO_TP_SIZE              atoi(getenv("SO_TP_SIZE"))
#define SO_BUDGET_INIT          atoi(getenv("SO_BUDGET_INIT"))
#define SO_SIM_SEC              atoi(getenv("SO_SIM_SEC"))
#define SO_MIN_TRANS_GEN_NSEC   atoi(getenv("SO_MIN_TRANS_GEN_NSEC"))
#define SO_MAX_TRANS_GEN_NSEC   atoi(getenv("SO_MAX_TRANS_GEN_NSEC"))
#define SO_MIN_TRANS_PROC_NSEC  atoi(getenv("SO_MIN_TRANS_PROC_NSEC"))
#define SO_MAX_TRANS_PROC_NSEC  atoi(getenv("SO_MAX_TRANS_PROC_NSEC")) 
#define SO_FRIENDS_NUM          atoi(getenv("SO_FRIENDS_NUM"))
#define SO_HOPS                 atoi(getenv("SO_HOPS"))

#define     SO_BLOCK_SIZE           10
#define     SO_REGISTRY_SIZE        100
#define     MSG_KEY                 12345
#define     SM_KEY                  ((key_t)5077)
#define     SEM_KEY                 ((key_t)5078)

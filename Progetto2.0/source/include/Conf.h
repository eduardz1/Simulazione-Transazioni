#include <stdio.h> 
#include <stdlib.h> 
/*SET GLOBAL VARIABLE INSIDE A  .h FILE WHITHOUT ANY VALUE   */ 
extern int    SO_USERS_NUM;            
extern int    SO_NODES_NUM ;
extern int    SO_BUDGET_INIT;
extern int    SO_REWARD;
extern int    SO_MIN_TRANS_GEN_NSEC;
extern int    SO_MAX_TRANS_GEN_NSEC;
extern int    SO_RETRY;
extern int    SO_TP_SIZE;  
extern int    SO_MIN_TRANS_PROC_NSEC ; 
extern int    SO_MAX_TRANS_PROC_NSEC  ;
extern int    SO_SIM_SEC  ;
extern int    SO_FRIENDS_NUM;  
extern int    SO_HOPS  ;




#define     SO_BLOCK_SIZE           10
#define     SO_REGISTRY_SIZE        100
#define     MSG_KEY                 12345
#define     SM_KEY                  ((key_t)5077)
#define     SEM_KEY                 ((key_t)5078)

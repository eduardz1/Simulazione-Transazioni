#include "Common.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>




void *printing(void *p);
/*void conf_value(); 
void conf_env();
*/

void print_Env();  
void simulation_print();

/*
struct exter_var{
 char **env_ptr; 
 char* so_user_num;
 char*so_nodes_num; 
 char*budget_init; 
 char*so_reward; 
 char*so_min_trans_gen_nsec;
 char*so_max_trans_gen_nsec; 
 char*so_retry ; 
 char*so_tp_size; 
 char*so_min_trans_proc_nsec; 
 char*so_max_trans_proc_nsec; 
 char*so_sim_sec; 
 char*so_friends_num; 
 char*so_hops; 
};
*/ 
/*

extern char * so_user_num;
extern so_nodes_num; 
extern budget_init; 
extern so_reward; 
extern so_min_trans_gen_nsec;
extern so_max_trans_gen_nsec; 
extern so_retry ; 
extern so_tp_size; 
extern so_min_trans_proc_nsec; 
extern so_max_trans_proc_nsec; 
extern so_sim_sec; 
extern so_friends_num; 
extern so_hops; 
*/
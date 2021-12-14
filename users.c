#include <reward.h>
#include <balance.h>

#define SO_BUDGET_INIT
#define SO_REWARD
/* 
 * NON active wait, the time is equivalent to the 
 * verification algorithms that happen in "real" blockchains 
 */
#define SO_MIN_TRANS_GEN_NSEC
#define SO_MAX_TRANS_GEN_NSEC
#define SO_RETRY

/*
 * Need to implement a way to send s transaction
 * signal, for example via a pipe
 */
void send_transaction(){
	int transaction_amount, reward, current_balance, retry;
	
	retry = SO_RETRY;
	current_balance = balance(SO_BUDGET_INIT);
	
	if(current_balance < 2) return;

	/* get random pid from user group */
	/* get random pid from nodes group */
	/* get random int from 2 to current_balance */
	reward = reward(transaction_amount);
	transaction_amount -= reward;
	
	while(retry--){
		/* transaction(node_pid, user_pid); */
		/* wait(rand(SO_MIN_TRANS_GEN_NSEC, SO_MAX_GEN_NSEC) */
	}
}

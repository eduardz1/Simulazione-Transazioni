#include "include/reward.h"

/*
 * we can't use pointers to share variable values, need to use shared memory resources
 */
void reward(int * transaction_value, int * reward)
{	
	/*
	 * directly modify the values of transaction_value and reward by 
	 * dereferencing the pointers
	 */
	*reward = ceil(((*reward * (*transaction_value))/100.0));
	*transaction_value -= *reward; 
	/* 
	 * function ceiling to round up the reward,
	 * given that the function can't generate a transaction if it has less than 2UC, 
	 * the amount of rewarded tokens should virtually always be >= 0
	 */
}

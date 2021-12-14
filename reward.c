#include <stdio.h>
#include <math.h>
#include "reward.h"
#define SO_REWARD

int reward(int transaction_value)
{
	float reward = ((SO_REWARD * transaction_value)/100);
	return ceil(reward); 
	/* 
	 * function ceiling to round up the reward,
	 * given that the function can't generate a transaction if it has less than 2UC, 
	 * the return should virtually always be >= 0
	 */
}

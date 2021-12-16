#include <stdio.h>
#include <math.h>
#include "reward.h"
#define SO_REWARD

void reward(int * transaction_value, int * reward)
{	
	/*
	 * directly modify the values of transaction_value and reward by 
	 * dereferncing the pointers
	 */
	*reward =ceil(((SO_REWARD * (*transaction_value))/100.0));
	*transaction_value -= *reward; 
	/* 
	 * function ceiling to round up the reward,
	 * given that the function can't generate a transaction if it has less than 2UC, 
	 * the amount of rewarded tokens should virtually always be >= 0
	 */
}

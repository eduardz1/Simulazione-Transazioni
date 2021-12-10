# Master
SO_USER_NUM
SO_NODES_NUM
SO_NUM_FRIENDS
SO_SIM_SEC

# Transactions
SO_HOPS

# Users
SO_BUDGET_INIT
SO_REWARD
SO_MIN_TRANS_GEN_NSEC
SO_RETRY

# Nodes
SO_TP_SIZE
SO_BLOCK_SIZE

makefile: master
	gcc -std=c89 -g -o0 -pedantic Master.c $1 $2 $3 $4 $5 Transactions.c $6 Users.c $7 $8 $9 $10 Nodes.c $11 $12 -o Master 

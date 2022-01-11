# TO DO LIST
*Sign which sections you're working on, tick when done and working*

## General Tasks
- [ ] **Eduard ->** implement semaphores to access "usersPID" and "nodesPID" structs
- [ ] implement message queue to send transactions (user communicates with node and ask to update ledger basically)
- [x] **Rick->**fix formatted_timestamp() in common.c
- [ ] finish every print function

## Master Specific
- [ ] handler can't have arguments, need to implement it without or use an extern variable (avoid if possible)

## User Specific
- [x] **Eduard ->** function to extract random userPID from "usersPID" array of "user" (needs to account status)
- [x] **Eduard ->** function to extract random nodePID from "nodesPID" array of "node" (needs to account status)
- [ ] function to calculate balance of a user based on transaction history
- [ ] signal handler for SIGINT
- [ ] signal handler for SIGUSR1 (sends a transaction)

## Node Specific
*Fill with something, I haven't taken a look at it yet*


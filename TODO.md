# TO DO LIST
*Sign which sections you're working on, tick when done and working*
## 30 Punti
- [ ] Inviare i nodi amici attraverso un messaggio di tipo msg_friend
- [ ] Decrement hops every time a transaction is sent (hops is already present in msg_trans, just need to literally write 1 line of code)

## General Tasks
- [ ] **Rick ->** fix formatted_timestamp() in common.c
- [ ] finish every print function
- [ ] clean general print formatting
- [ ] make ledger effectively print inside a ledger.txt, I've tried but it didnt't work
- [ ] fix all reported issues
- [ ] **Eduard ->** instead of sleeping for 1 second maybe we could make master sleep with a timer
- [ ] **Eduard ->** general cleanup of the code
- [x] **Eduard ->** implement semaphores to access "usersPID" and "nodesPID" structs
- [x] **Eduard ->** implement message queue to send transactions (user communicates
with node and ask to update ledger basically)
- [x] **Eduard ->** implement message queue to send transactions (user communicates with node and
ask to update ledger basically)
- [x] **Eduard ->** parser for a linked list (ledger)
- [x] **Eduard ->** function to append blocks to ledger

## Master Specific
- [ ] print every second of current_budget of every user and node (max 32 lines,
ordered by most budget)
- [ ] stop simulation when a certain number of users are dead
- [x] **Eduard ->** handler can't have arguments, need to implement it without or
use an extern variable (avoid if possible)
- [x] **Rick(ci prova)-->** print every second of num proc users and node active

## User Specific
- [x] **Eduard ->** function to extract random userPID from "usersPID" array of
"user" (needs to account status)
- [x] **Eduard ->** function to extract random nodePID from "nodesPID" array of
"node" (needs to account status)
- [x] **Eduard ->** function to calculate balance of a user based on transaction
history
- [x] **Eduard ->** signal handler for SIGINT
- [x] **Eduard ->** signal handler for SIGUSR1 (sends a transaction)
- [x] **Eduard ->** function that waits for a transaction when user is broke, behaviour without
this function is to spam non stop the check for (balance >= 2), wasting resources
- [x] **Eduard ->** message queue user-to-node 
## Node Specific
*Fill with something, I haven't taken a look at it yet*
- [x] **Eduard ->** receive transaction via user-node message queue, return if accepted or
declined
- [x] **Eduard ->** SIGINT handler, need to declare num transactions still in pool, balance

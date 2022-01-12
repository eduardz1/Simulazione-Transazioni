# TO DO LIST
*Sign which sections you're working on, tick when done and working*

## General Tasks
- [x] **Eduard ->** implement semaphores to access "usersPID" and "nodesPID" structs
- [ ] implement message queue to send transactions (user communicates with node and ask to update ledger basically)
- [x] **Rick->** fix formatted_timestamp() in common.c
- [ ] finish every print function
- [ ] parser for a linked list (ledger)
- [ ] function to append blocks to ledger
- [ ] ledger right now is allocated all at once, wasting memory, memory is cheap, i think it's fine like this but it can be implemented so that the shared memory of ledger is reallocated every time a block is added (quite a lot of changes to be made in that regard)

## Master Specific
- [x] **Eduard ->** handler can't have arguments, need to implement it without or use an extern variable (avoid if possible)
- [ ] print every second of num proc users and node active
- [ ] print every second of current_budget of every user and node (max 32 lines, ordered by most budget)

## User Specific
- [x] **Eduard ->** function to extract random userPID from "usersPID" array of "user" (needs to account status)
- [x] **Eduard ->** function to extract random nodePID from "nodesPID" array of "node" (needs to account status)
- [ ] **Eduard ->** function to calculate balance of a user based on transaction history
- [x] **Eduard ->** signal handler for SIGINT
- [ ] signal handler for SIGUSR1 (sends a transaction)
- [ ] function that waits for a transaction when user is broke, behaviour without this function is to spam non stop the check for (balance >= 2), wasting resources
- [ ] **Eduard ->** message queue user-to-node 

## Node Specific
*Fill with something, I haven't taken a look at it yet*
- [ ] receive transaction via user-node message queue, return if accepted or declined
- [ ] SIGINT handler, need to declare num transactions still in pool, balance


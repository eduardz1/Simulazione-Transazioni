# Simulazione-Transazioni
Progetto del corso di Sistemi Operativi 2021-2022 
 ---
## MASTER
 Il processo master si occupa di:
 - inizializzare gli oggetti [ipc](##IPC), il signal handler al fine di poter usare il SIGINT per interrompere la simulazione a proprio piacimento usando la shortcut _CTRL-C_
 -  Crea i nodi usando una fork e assegna a ciascun nodo una coda di messaggi, fa un execve a ./nodes passandogli come _arguments_ glii ID degll oggetti IPC meno la coda di messaggi
 -  Una volta che sono stati creati tutti i nodi, creerá una lista amici,per ciascun nodo, la quale verrá mandata un amico alla volta sulla coda di messaggi corrispondente
 -  Crea gli _user_ sempre usando una fork come per i nodi fa un execve a ./users passandogli come _arguments_ gli ID degll oggetti IPC
---


 ---
 ## NODES
 Ogni nodo ha una coda di messaggi corrispondente, questa viene attaccata usando come key il PID del processo nodo. I nodi creano un blocco con reward a 0, in seguito inserisce nei blocchi la lista delle transazioni da elaborare. Inizailizza la _transaction pool_ che é una linked list 

 
 
 
 ---
 ## USERS
 Il processo _users_ quando viene creato, attacca gli oggetti oggetti ipc e gli ID passati come arguments, entra in un loop nel quale imposta un tempo tra SO_MIN_TRANS_PROC_NSEC e SO_MAX_TRANS_PROC_NSEC,da questo punto calcola il bilancio, e imposta lo stato, se lo stato é su _broke_ viene eseguita una sleep(2). 
 
 ---
  ## IPC
Gli ipc object che vengono inizalizzati nel progetto sono:
- 2 semafori - 
	- il 1 per gestire la scrittura tra NodesPid e UserPid che sono due array di struct che raccolgono le informazioni relative agli _users_ e ai _nodes_
	- il 2 é utilizzato per scrivere sul ledger  
- Tante code di messaggi quanti sono i nodi +1, che é quella del libro mastro(ledger) per ricevere le transazioni scartate dal numero di hops.
- Shared memory un array di struct che contiene i parametri di tutti i nodi e di tutti gli _users_

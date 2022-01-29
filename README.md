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
 Il processo _users_ quando viene creato, attacca gli oggetti oggetti ipc e gli ID passati come arguments, entra in un loop nel quale imposta un tempo tra SO_MIN_TRANS_PROC_NSEC e SO_MAX_TRANS_PROC_NSEC,da questo punto calcola il bilancio, se questo é maggiore o uguale a 2 imposta lo stato a _alive_, estrae il PID di un _user_ casuale e il PID di un _nodes_ casuale e genera una transazione casuale compresa tra i valori di _current balance_  
 se il bilancio é minore lo stato viene impostato a _broke_ e non viene inviata alcuna transazione e viene avviata una sleep(2).
 In caso un processo non riesca a inviare alcuna transazione per un numero di volte pari a quello di _SO RETRY_ allora termina la propria esecuzione
 
 ---
  ## IPC
Gli ipc object che vengono inizalizzati nel progetto sono:
- 2 semafori - 
	- il 1 per gestire la scrittura tra NodesPid e UserPid che sono due array di struct che raccolgono le informazioni relative agli _users_ e ai _nodes_
	- il 2 é utilizzato per scrivere sul ledger  
- Tante code di messaggi quanti sono i nodi +1, che é quella del libro mastro(ledger) per ricevere le transazioni scartate dal numero di hops.
- In shared memory un array di struct che contiene i parametri di tutti i nodi e di tutti gli _users_


--- 
## SCELTE IMPLEMENTATIVE
Libro Mastro --> Implementato come un array di blocchi e viene inizializzato in _master_ come segmento di shared memory

Common.h --> É un header dove sono definiti la maggior parte delle macro e delle _struct_ usati all'interno del progetto in modo che non siano sparse e difficile da trovare in caso di modifiche da attuare in seguito alla loro definizione.


Funzioni di stampa --> Tendenzialmente sono raggruppate in un unico file _print.c_ e corrispettivo header per diminuire la confusione   Le funzioni di stampa si occupano di mostrare a schermo attraverso le apposite funzioni le informazioni di output,lo stato delle transazioni,le tempistiche che impiegano,numero di processi nodo e user attivi in quell'istante, stato dei processi se sono broke, alive.

Implementazione dei nodi amici --> I nodi amici sono stati implementati come un array, vengono mandati singolarmente al processo master, e ogni 

Lettura delle configurazioni a run-time-->Viene gestita da un parser e il quale crea due tokens, una stringa e in numero , in maniera tale da poter confrontare la stringa che ha letto e in modo tale da assegnare i valori letti alla struct _parameters_ salvata in _shared memory_
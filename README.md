# Simulazione-Transazioni
Progetto del corso di Sistemi Operativi 2021-2022 

 ---
## Master
 - Inizializza gli oggetti [IPC](##IPC)
 - Inizializza il signal handler al fine di poter usare il `SIGINT` per interrompere la simulazione in maniera pulita e a proprio piacimento usando la shortcut `CTRL-C`
 - Crea `SO_NUM_NODES` *nodes* usando una fork; il *children* generato assegna a ciascun nodo una coda di messaggi personale che utilizza il PID stesso del nodo come key ed effettua una *execve* a *./nodes* passandogli come _arguments_ gli ID degll oggetti IPC generati in precedenza
 - Crea una lista amici per ciascun nodo, la quale verrà inviata un amico alla volta sulla coda di messaggi corrispondente
 - Crea `SO_NUM_USER` _users_ usando una fork come per i nodi e facendo una *execve* a *./users*, passandogli come _arguments_ gli ID degll oggetti IPC
- Imposta un *alarm()* a `SO_SIM_SEC`, il `SIGALRM` verrà gestito allo stesso modo del `SIGINT`
- Effettua una *fork()*
	-  il master inizia a mostrare a schermo ogni secondo il numero di processi attivi nonché quelli con bilancio più significativo
	-  il figlio generato invece si metterà in ascolto per transazioni inviate sulla queue privata del *master* nella quale sono inviate le transazioni che non sono riuscite a trovare un *node* che le processasse entro `SO_HOPS`, quando ne riceve una crea un nuovo nodo (fino ad un massimo di `SO_NODES_NUM` nodi extra), nel caso ci siano giá troppi nodi extra generati la transazione viene semplicemente scartata[^1]
 
 ---
 ## Nodes
 - Attacca la propria coda usando come key il proprio PID
 - Attacca gli oggetti IPC a partire dagli ID che vengono passati come arguments
 - Inizializza un `SIGINT` handler che rimuoverà la coda associa al *node* alla terminazione
 - Si mette in ascolto per riempire la propria lista amici, il processo non può partire finché il master non gli invia tutti i *friends*
 - Inizializza la *transaction pool* come *linked list*
 - Entra in un *loop* nel quale inizia il *fetch* dei messaggi inviati sulla propria *queue* aspettando di ricevere una transazione da aggiungere alla *transaction pool*[^2], dopodiché controlla -con flag `IPC_NOWAIT`- se sono presenti nuovi *friends* da aggiungere alla lista di amici, in caso affermativo realloca l'array di friends in maniera tale da poter far posto a un nuovo *node* amico. 
	 - Periodicamente[^3], e se la _transaction pool_ è piena, la transazione che è presente nella *pool* da più tempo viene inviata a un amico scelto casualmente, se invece la transazione ha già esaurito gli _hop_ dispnibili, questa viene inviata al *master*, il quale si occuperà di creare un nuovo nodo. 
- Dopodiché il nodo controlla se ha abbastanza transazioni per creare un nuovo blocco, in caso affermativo effettua una *fork()* 
	- il figlio crea e si occupa di generare e confermare le transazioni del nuovo blocco, dopodiché efettua una *sleep()* che simula l'elaborazione dei blocchi
	- il padre ricomincia il loop in maniera tale da poter continuare a fare il *fetch* dei messaggi in parallelo con l'elaborazione del blocco  
 ---
 ## Users
 - Attacca gli oggetti oggetti ipc a partire dagli ID passati come arguments
 - Inizializza il `SIGINT` handler che aggiornerà il bilancio del nodo in maniera tale da avere una rappresentazione più accurata del valore a terminazione[^5]
 - Entra in un loop nel quale calcola il proprio bilancio a partire dalle transazioni confermate sul ledger sottraendo quelle ancora in uscita, se questo é $\geq2$:
	 -  imposta il proprio stato ad *alive*, estrae il PID di un _user_ casuale e il PID di un _node_ casuale e genera una transazione di *amount* compreso tra i valori 2 e _current balance_ e la invia allo *user* estratto sfruttando il *node* estratto, se non riesce ad inviare la transazione generata diminuisce il valore di retry, nel caso in cui questo arrivi a 0 cambia status in *dead* e termina la propria esecuzione. A prescindere dalla riuscita dell'invio di una transazione lo *user* a questo punto effettua una *sleep()*[^4]
	- se il bilancio é minore lo stato viene impostato a _broke_

 In caso il balance sia sufficiente per essere processato da un nodo invia la transazione, in caso contrario diminuisce il numero di retry, se il numero é 0 e il loro stato viene impostato a _dead_ e terminano la loro esecuzione.
 
 ---
  ## IPC Objects
Gli ipc object che vengono inizalizzati nel progetto sono:
- 2 semafori - 
	- il 1 per gestire la scrittura tra NodesPid e UserPid che sono due array di struct che raccolgono le informazioni relative agli _users_ e ai _nodes_
	- il 2 é utilizzato per scrivere sul ledger  
- Tante code di messaggi quanti sono i nodi +1, che é quella del libro mastro(ledger) per ricevere le transazioni scartate dal numero di hops.
- In Shared memory un array di struct che contiene i parametri di tutti i nodi e di tutti gli _users_


--- 
## SCELTE IMPLEMENTATIVE
Libro Mastro --> Implementato come un array di blocchi e viene inizializzato in _master_ come segmento di shared memory

Common.h --> É un header dove sono definiti la maggior parte delle macro e delle _struct_ usati all'interno del progetto in modo che non siano sparse e difficile da trovare in caso di modifiche da attuare in seguito alla loro definizione.


Funzioni di stampa --> Tendenzialmente sono raggruppate in un unico file _print.c_ e corrispettivo header per diminuire la confusione   Le funzioni di stampa si occupano di mostrare a schermo attraverso le apposite funzioni le informazioni di output,lo stato delle transazioni,le tempistiche che impiegano,numero di processi nodo e user attivi in quell'istante, stato dei processi se sono broke, alive.


Implementazione dei nodi amici --> I nodi amici sono stati implementati come un array, vengono mandati singolarmente al processo *master*, e ogni

Lettura delle configurazioni a run-time-->Viene gestita da un parser e il quale crea due tokens, una stringa e in numero , in maniera tale da poter confrontare la stringa che ha letto e in modo tale da assegnare i valori letti alla struct _parameters_ salvata in _shared memory_

[^1]: Utilizzando un sistema più complesso rispetto alla shared memory di linux si potrebbe allocare in maniera dinamica lo spazio necessario per memorizzare le informazioni relative ai nodi extra in maniera tale da rimuovere il limite massimo di *nodes* extra. Il valore scelto, pari a quello di `SO_NODES_NUM` è puramente arbitrario
[^2]: Questa implementazione causa deadlock nel caso in cui due amici vengano inviati uno dietro l'altro sulla coda di messaggi, una maniera per risolvere è di settare la flag `IPC_NOWAIT` anche alla *msgrcv* delle transazioni ma così facendo le performance peggiorano considerevolmente. Il rischio che accada è estremamente basso, l'unico caso realistico in cui potrebbe accadere è quello nel quale tutti gli *user* sono *broke* e il master crea ed invia allo stesso *node* due amici ma è un *edge case* abbastanza estremo (mai capitato nella pratica) e dovuto al fatto che periodicamente bisogna inviare una transazione ad un amico a prescindere da quante se ne hanno nella *transaction pool*
[^3]: ogni 20 check ma il numero è completamente arbitrario
[^4]: utilizziamo una *clock_nanosleep()* anziché la *nanosleep()* per via di un bug segnalato su quest'ultima nella *man page*, il tempo rimasto ancora della sleep viene salvato in una struct separata in maniera tale che questo venga completato anche se lo *user* viene forzato ad inviare istantaneamente una transazione con `SIGUSR1`
[^5]: a questo punto teoricamente gli user dovrebbero sommare anche le transazioni ancora presenti nella propria queue perché non c'è più speranza che queste vengano scritte sul libro mastro, in pratica però queste transazioni non sono state "elaborate" e quindi confermate dai nodi quindi le riteniamo come inaffidabili
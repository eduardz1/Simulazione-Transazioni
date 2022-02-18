INDEX

 -Variabili Globali
-Gestione dei Processi 
-IPC
-Semafori (Bini)
-Shared Mamory 
-Code di Messaggio
-Nodi
-MakeFile
-Studiare il ReadMe




  ### Varibile Globali 

  -Parte di codice nella quale vengono definiti funzioni o costanti in cui non coincede con il main o altre funzioni, in cui implica una allocazione di memoria.
  
## Come si riconoscono ?
 <p>
      *Dichiarate al di sopra del codice. 
      *Non presentano il corpo ma solo un ";" finale
      *Possono essere definite con "extern" --> ex: extern int due =2; 
      *Possono essere dichiarate sopra il main,ma definite in altre porzioni di codice    
     </p>
	
### A Che Servono? 
 <p>
     * Servono per eveitare il passaggio di parametri per ogni funzione 
     </p>
###  Sono sicure ? 
<p>
      *Con una dichiarazione standard non molto poichè sono visibili e modificabili da tutti,per ovviare a questo problema basta dichiararle come "static " in modo che sia visibile solo nello scope di definizione,rendendole permanente e protette.

</p>
### Direttive Condizionali 
<p>
	*Queste espressioni permettono di evitare la compilazione di alcune parti di codice ponendo delle condizioni 
     /*Esempio*/
     -#if,#else,#elif,#endif --->Esistono direttive condizionali di 								compilazione che si
								 basano sul fatto che una certa macro sia definita o meno 
											
</p>
### GESTIONE DEI PROCESSI


#####  INTRO

Un processo  può essere definito come "ogni singolo programma in esecuzione".
Unix assegna ad ogni processo un proprio _PID_ 
(Process ID)
## Creazione processo
Per creare un nuovo porcessso su unix è quello di dublicare uno esistente ,utilizzando  le "_fork()_ ,che eredita dal processo padre i suoi attributi continuando ad eseguire il codice del processo Padre,differescono per Pid(pid_t Sys call) e PPID e risorse a run-time diverse. 
Per terminare un processo si utilizza la _exit()_  
e la terminazione viene comunicata al processo padre tramite segnale .

i processi possono appunto modificare il loro stato e in Unix abbiamo visto che questo si può fare tramite i comandi ,"wait(),sleep()"

All'avvio del Os c'è un solo processo visibile chimato _init()_ che è l'antenato comune di tutti i processi esistenti in quel momento. 

initi() crea i processi getty() responsabili di gestire i login degli utenti.

fork()--> questo comando restituisce due valori 
              differenti nel processo padre e figlio 
			 *  Parent-->Il PID del figlio (se non sono presenti errori)
			 * child: restituisce 0.

## Terminazione 
Quando un figlio termina, il padre riceve un segnale  
SIGCHLD ed il figlio aspetta che il suo codice di  terminazione status sia accetta  o.  
Solo gli 8 bit m eno significat ivi di status sono ut ilizzat i,  
così i valori del codice vanno da 0 a 255.  
Un processo padre accetta il codice di terminazione di un figlio  eseguendo una wait()-->pid_t wait(NULL) che restituirà: 

   * -1 --> se il processo figlio ha terminato 
   * Il PID dei processi figli che hanno terminato 

Ma vi è altri metodi per effettuare la terminazione di un porocesso: 

* System Call-->exit(status)
* MACROS--> EXIT_SUCCESS(0) || EXIT_FAILURE(1)
* Signal-->che puoò essere un "Ctrl+c"--> come nel progetto


### Processi Zombie 
Un processo che termina non scompare dal sistema fino a che il padre non ne accetta il codice di terminazione,finchè ciò non accade il proccesso rimarrà li  senza codice,dati o pila di stack ma continua ad avere un suo _PCB_ occuppando un quantità minima di risorse di sistema.
Il processo terminerà definitavemente quando il processo padre eseguirà una wait() per accettare la sua terminazione.


# IPC(Inter-Process-Comunication)
<p>
  Le  IPC permettono la comunicazione tra i processi  permettendo lo scambio di dati,quello che abbiamo trattato nel corso sono 
  -PIPE
  -FIFO
  -Message queue
  -Shared Memory
  -Semafori 
</p>
## PIPE 
Le pipe permettono la comunicazione tra processi in modo unidirezionale e sequenziale
### Limitazioni

– possono essere stabilite soltanto tra processi imparentati (es., un processo  
ed un suo “progenitore”, o tra due discendenti di un unico processo)  
– Non sono permanenti e sono distrutte quando il processo che le crea termina  
### Soluzione: 
assegnare un nome unico alla pipe: named pipe dette anche FIFO.  
• Funzionamento identico, ma il riferimento avviene attraverso il nome anzichè  
attraverso il file descriptor.  
• Esistono fisicamente su disco e devono essere rimossi esplicitamente con unlink


## FIFO 

• Si creano con mknod (l’argomento dev viene ignorato)  
mknod (nome, S IFIFO|mode, 0)  
– valore di ritorno: 0 in caso di successo, -1 in caso di errore.  
• apertura, lettura/scrittura, chiusura avvengono come per un normale file  
• Possono essere usate da processi non in relazione, in quanto il nome del file è  
unico nel sistema.  
• Le operazioni di I/O su FIFO sono atomiche  
• I/O normalmente bloccante, ma è possibile aprire (con open e flag O NDELAY)  
un FIFO in modo non bloccante. In tal modo sia read che write saranno non  
bloccanti.  
• Utilizzabile anche la funzione mkfifo()

## Message queue

• Un messaggio è una unitàdi informazione di dimensione variabile, senza un  
formato predefinito  
• Vengono memorizzati nelle code, che vengono individuate dalla chiave

• Crea una coda di messaggi data la chiave key (di tipo long) se  
– key = IPC PRIVATE, oppure  
– key non è definita, e flag & IPC CREAT è vero.
• Hanno significato solo i flag di lettura e scrittura.
• buffer è un puntatore a una struttura definita in sys/msg.h contenente  



## SHARED MEMORY 

• Due o più processi possono comunicare anche condividendo una parte del loro  
spazio di indirizzamento (virtuale).  
• Questo spazio condiviso è detto memoria condivisa (shared memory), e la  
comunicazione avviene scrivendo e leggendo questa parte di memoria
• I parametri hanno lo stesso significato di quelli utilizzati da msgget.  
• size indica la dimensione in byte della regione condivisa.
• Una volta creata, l’area di memoria non è subito disponibile  
• Deve essere collegata all’area dati dei processi che vogliono utilizzarla
• shmaddr indica l’indirizzo virtuale dove il processo vuole attaccare il seg-  
mento di memoria condivisa.  
• Il valore di ritorno rappresenta l’indirizzo di memoria condivisa effettivamente  
risultante

• buffer è  un puntatore a una struttura definita in sys/shm.h

## Sincronizzazione tra Processi  
• I semafori permettono la sincronizzazione dell’esecuzione di due o più processi  
– Sincronizzazione su un dato valore  
– Mutua esclusione  
• Semafori SystemV:  
– piuttosto diversi da semafori classici  
– “pesanti” dal punto di vista della gestione  
• Disponibili varie API (per es. POSIX semaphores)


## MAKEFILE

L'utility "make " non fa parte del gcc,poichè è un program manager in grado di gestire un gruppo di moduli di un programma per la compilazione di essi
molto utile se  dobbiamo linkare molti file tra di loro e quindi con una compilazione risolviamo il tutto .
* Make viene influenzato da alcune varibili d'ambiente che sono: 
     * CC--> stringa che ha il nome del 
                   compilatore.
	 * CFLAGS--> is added to the compilation  
                           flags(in inglese rende meglio )
	  * LDFLAGS--> aggiunto nelle "linking flags" 				   




## QUALCHE APPUNTO LEZIONE BINI  PER CAPITOLO 


## Minimal Program 

-Le variabili in sono porzioni di memoria 
 e quindi la definiamo daremo un quantitativo di memoria ad essa e la posizione dove viene allocata 

-La memoria in C è astratta e assomiglia ad una lunga sequenza di Byte con tanto di indirizzo di riferimento 

-sizeof() --> restituisce la dimenzione della 
                      variabile d'interesse

## ARRAYS

-Gli array non sono classi (come in java ) ma è una porzione di memoria dove vengono allocate variabili con lo stesso tipo 

-La lunghezza di un Array non è salvata in una struttura dedicata ma è possibile ricavarla attraverso vari modi ,come salvandolo in un altra varibile 

-I metodi e gli oggetti non esistono in C 

-Il termine "String" in C denota come un Array di caratteri

-Convertire String-->Int  --> *atoi()*
-String --> floating poin --> atof()
-fgets() --> leggere stringhe di caratteri con 
                    una certa dimensione  


## Operators And Control
-Gli operatori Bit-wise sono uguali a java 
-I boolean non esistno--> usiamo le flag 


## Pointers To Memory

-I puntatori possono essere visti come un indirizzo di memoria 

##### Defenrenziare 
Una delle oprazioni che possiamo effettuare con i puntatori è la deferenzazione ,consiste nel  recuperare  effettivamente i dati a cui il puntatore  punta(Perdonate il gioco di parole).


### Segmentation Fault
-è un errore che si verifica quando l'utente si asoetta un operazioende di lettura o scrittutra su una porzione di memoria a cui però non riesce ad  accedervi 


## Understanding All 2519 options of Gcc

-Il codice diventa eseguibile dopo una serie di passaggi antecedenti che sono ben 4.

#### Pre-Processing
In questo primo passaggio vengono riconosciuti e inseriti tutti gli header(.h),Macro ecc che saranno presenti nel codice .
- Per invocare solo il pre processore basta  usare "gcc -E NomeFile"
- "gcc -S NomeFile"Compila e stampa il codice in formato assembly. 

#### Compiling
Il codice C viene tradotto in codice Assembly 

#### Assembling 
L'assembler traduce il codice assembly in linguaggio macchina  o object code (Non so come tradurlo )

#### Linking 
In questo ultimo step vengono integrate tutte le librerie o sottoprogrammi presenti nel codice .
Linker genera un file eseguibile contenente le parti  di codice macchina copiate dalle librerie

-Le direttive per il Pre-Processor sono indicate con "#"  tipo:#define ,#include,#if.

## Macros For Debugging

Sono presenti delle macro specifiche per aiutarsi durante la fase di debugging: 

- __FILE__  -->  Stringa con il nome del 
                   file in cui la macro appare 
                   usato con programmi con molti file. 

* Line-->    Punto nel codice in cui la 
                   macro appare.
* Time/Date-->Tempo/Data della 
                            compilazione .


### Scope Of Variables And Storage Classes


### Allocazione Dinamica 
Per allocare una parzione di mememoria in un programma possiamo impostare delle quantità di memoria specifiche ia nostro piacimento,ciò attraverso alcune funzioni come
* malloc()-->  allocazione di una 
                       quantità di memoria con un certo size,e restituisce un puntattore a quella memoria.
* calloc()-->   alloca una porzione di 
                       memoria grande tanto quanto gli elementi che 
					   la compongono.
* realloc()-->Cambia il size di 
                    memoria che abbiamo già allocato in un altra porzione di codice.  

* free()--> Permette il rilascio della 
                  memoria che abbiamo allocato.

L'allocazione dinamica è meglio rispetto alla statica poichè viene utilizzata meno mememoria e viene usata solo quando effettivamente è necessario.


## More Pointers
i puntatori sono varibili quindi è possibile creare un array con un puntatore così in un base alle nostre necessità posssiamo usare un dato specifico
* E' possibile pure creare devi puntatori  a puntatori così da avere quel determinato dato anche altre porzioni di codice non modificando il suo puntatore originale 
* E' anche possibile creare un puntatore ad una funzione in modo da manipolarla a nostro piacere. 


## Files

-In C è possibile effettuare operazioni su file ciò attraverso le apposite funzioni della librerio "stdio.h"
-E' possibile anche gestire lo stream dei file ,cioè modificare la sorgente del nostro input(di default è la tastiera) attraverso alcune funzioni integrate nella librerio "stdio.h"
 * stdin (standard input che di default è la tastiera)

* stdout (standard output che di default è lo schermo)

* stderr (standard error che di default è lo schermo)


###  Buffering
Le oprazioni di scritture/lettura sono operazioni che richiedono un certa quantità di tempo,quindi per ottimizzare i 

###  File Descriptors 

I file descriptor sono degli Int che hanno come scopo quello di indentificare la sorgente o la destinazione di una sequenza di Byte, e si utilizza il comando "open()"

* stdin,stout,sterr sono dei classici flussi gestiti dal sistema operativo, infatti ad ogni comando viene associato un File descriptor di riferimento(un int ) nello specifico: 
   -stdin()=0,stdout()=1,stderr()=2.   

   #### Opertura/chiusura dei Fd

   Per effettuare l'apertura di un "fd" è possibile utilizzare il comando "open()" che genererà un nuovo fd.
   Sono presenti delle specifiche macro da 1 bit che  informano sullo status del file e sono: 
   0_RDONLY,0_WRONLY,0_RDWR-->Devono essere include per 
                                                               forza una tra queste

  0_APPEND-->  La modalità _append_ permette appunto di aprire 
                            un file in maniera che sia possibile scrivere in coda al contentuto attuale del file.

   0_CREAT-->  Genera un file se non esiste.

   0_TRUNC-->Se il file esiste già viene troncato. 


   int close(int fd);-->Dopo aver utilizzato il fd va anche chiuso.

   ###### Lettura da fd

   ssize_t read ( int fd , void * buf , size_t size )--> Restituisce il numero di Byte che ha appena letto che in generele è più piccolo della dimensione. 
   Restituisce una flag di stato alla fine cioè : 

   0-->EOF raggiunto (operazione di chiusura andata bene )
   -1--> Sono stati riscontrati degli errori durante l'operazione. 

##### Scrittura su fd 

ssize_t write ( int fd , const void * buf , size_t  size );-->Scrive la dimensione dal buffer al fd,questa operazione avviene subito con fprintf().
E' consigliato evitare di effettuare mix tra fprintf e write sullo stesso stream o fd,poichè l'output generato da fpritnf() è ritardato rispetto alla write. 


## MODULES IN C 
Effettuare programmi molto lunghi può portare delle problemmatiche durante la fase di debug ma anche nella stessa stesura del codice,il linguaggio C ci propone una soluzione che sono i moduli,in cosa consiste? 
Consiste nella semplice divisione di programmi che già abbiamo sviluppato e perfezionato che risiede in un file tutto suo.
I moduli possono essere di tre tipi: 
* Interface(.h)--> detti anche header file in cui sono presenti delle struct o includono in loro stessi delle librerie ,ma non è codice eseguibile da solo vedetelo un po' come un pezzo di un puzzle-->Solo è inutile insieme ad altri è fondamentale. 

* Module code(.c)-->contine l'implementazione di una funzione già presente in un header file,può anche contenere variabili globali di lettura o scrittura ma *NON CONTIENE IL MAIN*.
   Per compilarlo bisongna utilizzare su terminale il comando "gcc -c"che restituirà un file object. 

* Application code-->possono essere integrati dei maduli di tipo header,al suo interno presenta la funzione di main,la compilazione avviene con il collegamento tra gli object file e gli header integrati al suo interno.  
*--*> ESEMPIO-->gcc application.c module-name.o -o application

 ## Variabili Nei Moduli 

 I moduli nei maggiori dei casi utilizzano varibili globali in modo da essere interfacciato in entrambi i moduli, C  propone due dichiarazioni : 

 * double precision--> dichiare nel file.c 
 * extern double precision--> dichiarare nell'header file 
 Questi sono i modi per farsi che il compilatore capisca dell'esistenza della variabile senza allocare nessuna porzione di memoria,nel caso in cui avremo sbagliato la procedura di dichiarazione ci verrà restituito un errore. 

## Librerie

*Librery*--> con questa espressione facciamo riferiemento ad un insieme di moduli .

Le librerie possono essere di due tipi: 
 * Static(.a)--> Il codice viene inserito in un eseguibile anche di 
                   importanti dimensioni.
* dynamic(.so)-->l'eseguibile non contine codice,e quando 
                     vengono richiamate funzioni o altro,ci sposteremo segmento di esecuzione differente.

#### Object dump 
* objdump mostra i contenuti di un object file,l'estenzione utilizzata è "ELF(Executable and Linkable Format)".

  * Per visualizzare l'assembly di un object file utilizziamo 
  "objdump -d filename.o"
  * Per ricompilare "gcc -c -g filename.c".
  * codice sorgente e codice assembly "onjdump -S filename.o"


## Debugging by gbd

Il gbd è uno strumento di debugging molto utile per scovare le problematiche che risiederanno all'interno del nostro codice(si perchè ci saranno sempre).

Ora una serie di comandi utili per richiamare il gbd o funzioni che gli appartengono: 
* gbd-->richiamiamo il dubugger.
     * List--> lista il codice sorgente dalla stringa che vorremo 
                    noi
	* break-->inserisce un breakpoint alla (sempre nella stringa 
                    da noi dichiarata) 
   * run--> esegue dal primo breakpoint
   * ALTRI COMANDI--> man gbd  


## Signals
I segnali sono degli interruttori software per la gestione dei processi e possono essere generati dagli utenti dal software o anche dall'hardware per qualche evento specifico.

**ELENCO DI SEGNALI PRESI PARI PASSO DALLE SLIDE :)**
 * SIGFPE “Floating Point Exceptions” such as division by zero.  
 * SIGILL trying to execute an “Illegal instruction” .
 * SIGINT used to cause program interrupt (Ctrl+C)  
 * SIGKILL causes immediate program termination.  
 * SIGTERM polite version of terminating a program (SIGTERM can be  
handled by the user) 
* SIGALRM received when a timer (set by alarm(int seconds),  
timer_create(...), or other calls) has expired .
 * SIGCHLD sent to a parent when a child terminates.  
 * SIGSTOP/SIGCONT stop/continue a process . 
 * SIGUSR1/SIGUSR2 user-defined signals.

 Ogni segnale ha degli handler di default: 
 * Term-->Termina il processo 
 * Ign-->Ignora il segnale
 * Core-->Termina il processo e genera un dump file che contiene la quantità di memoria dei processi durante l'esecuzione 
 * Stop
 * Cont-->Riprende il processo se stato stoppato. 


### User-defined 
User-defined è un signal handler, chiamato "asynchronously"
quando viene utilizzato all'interno del codice avvengono alcune cose cioè:
* Lo sato dei processi viene salvato 
* Le funzione dell'handler sono eseguite 
* Lo stato del processo viene ripristinato  

##### synchronous 
 Il signale quando viene "consegnato" non viene avvisato in qualche modo e quindi può posizionarsi un uno stato d'attesa finchè non viene eseguita 


## Semaphores
I semafori sono molto utili per evitare i processi siano fuori sincronia fra di loro e per effettuare operazioni di scheduling.

#### Come funzionano? 
Per ogni semaforo il kernel memorizza v(s) un valore sempre >=0
che rappresenta il valore delle risorse protette da "s"
  * Ogni processo può effettuare le segueti azioni su un semaforo.
    * Inizializzare una varibile con un int che presenta il numero di risorse disponibili.
    * v(s) viene icrementato se ci sono risorse che vengono utilizzate 
    * se v(s)= 0--> un processo può richiedere risore 
                       * -->Se si hanno tanti processi che aspettano il via.

## System V Semaphore 

* System V implementa un array di semafori in cui ogni azione è atomica -->RICORDA: i semafori sonbo molto utili per processi 
                                        che utilizzano molte risolte .

-La system call(semget) restitusce un identificativo di un array di semafori che viene associato alla chiave che abbiamo impostato. 

* semflag--> include questa serie di syscall: 
    * IPC_CREAT--> crea un semaforo che associato ad una key,se non esiste, se esiste già restituisce il semaforo associato a quella chiave.
    * IPC_EXCL()-->verifica se  IPC_CREAT sta funzionando 
                               correttamente(SI USANO INSIEME).

I semafori sono oggetti persistenti quindi sopravvivono ad una chiusare di una processo quindi devono essere cancellati direttamente.

### Accesso/Rilascio risorse 
* Per accedere ad una risorsa il semaforo necessità di essere decrementato ,se non ci son opiù risorse il processo si blocca 
* Per rilasciare una risorsa il semaforo deve essere incrementato  --> Non bloccare un processo durante questa 
                                 operazione.
* sem_num-->Numero di semafori nell'array. 



### Protezione Sezione Critica 

Se abbiamo molti processi che vogliono effettuare un operazione di scrittura nella stessa area di memoria condivisa per evitare di perdere la consistenza dei dati facciamo runnare un processo alla volta. 
Utilizzando un semaforo eviteremo che i processi o vanno in starvation o in deadlock o in qualche altra schifezza 




## IPC--> BINI  

## QUALCHE APPUNTO IN PIU' 

* Se allochiamo con una malloc la memoria allocata non sarà visibile ai processi poichè lavoriamo sull'heap,e il processo sarà "forkato".
* I processi non condivideranno la stessa memoria, e i puntatori rappresentano gli indirizzi logici 

*LE SHARED MEMORY SONO PIU' VELOCI*  
-->PRO: Velocità generale,i dati scritti sono subito disponibili, semplicità 
-->Cons: alto rischio di inconsistenza

## CIclo di vita
* Creazione -->shmget()= size di memoria da "allocare". 
* Attaching-->Dopo che abbiamo definito la memoria può essere 
                        subito sfruttata da un processo,e può essere visibile agli altri processi dello stesso segmento.
 * Detaching--> Separazione dal processo,però la memoria non è 
                           visibile ma esiste ancora(persistente) .
* Deallocamento--> Cancellazione della memoria che abbiamo 
                                 allocato.


## Creazione 
La system cal-->"int shmget ( key_t key , size_t size , int shmflg )"
restituisce l'identificatore della memoria condivisa associata a quella key--> La memoria assegnata sarà il doppio del "PAGE_SIZE". 

# TRANSAZIONI
Una transazioni é caratterizzata dalle seguenti informazioni:
>_timestamp_ --> della transazione in nano-secondi (c'é la funzione clock_gettime()) <br>
>_sender_ --> é l'utente che ha generato la transazione <br>
>_receiver_ --> utente destinato alla somma <br> 
>_quantitá_ --> di denaro che viene inviato <br>
>_reward_ --> denaro pagato dal sender al nodo che processa la transazione <br>


# PROCESSI UTENTE:
-Deve esserci un budget iniziale </br>
 Bisogna fare:<br>
 >1<br>
 > le entrare registrate nelle transazioni del libro mastro </br>
 > le uscite registrate nelle transazioni del libro mastro </br>



<center> CONDIZIONI POST-TRANSAZIONI </center> 

``` c
if(bilancio>=2){ 
il processo estrae a caso:
- Un altro processo utente destinatario a cui inviare il denaro
- Un nodo a cui inviare la transazione da processare
- Un valore intero compreso tra 2 e il suo bilancio suddiviso in questo modo:
* il reward della transazione==percentuale SO_REWARD(che sarebbe la percentuale) del valore estratto, con un minimo di 1
* l'importo della transazione sarà uguale al valore estratto sottratto del reward
    
} else {  // il bilancio è minore di 2
- il non invia processo non invia alcuna transazione 
}
```

>2<br>
> Invia al nodo estratto la transazione e attende un tempo in nanosecondi estratto casualmente tra _SO_MIN_TRANS_GEN_NSEC_ e _SO_MAX_TRANS_GEN_NSEC_

>*NB*
> - Un processo utente deve generare una transazione anche in risposta ad un segnale ricevuto
> - ma se un porcesso non riesce ad inviare alcuna transazione per SO_RETRY volte *consecutive*, allora termina la sua esecuzione notificando il processo master


# PROCESSI NODO


Ogni processo nodo memorizza _privatamente_ la lista di transazioni ricevute da processare (_transaction_pool_) che al massimo puó contenere SO_TP_SIZE transazioni, con _SO_TP_SIZE>SO_BLOCK_SIZE_.

>NB se la _transaction_pool_ del nodo é piena, allora ogni ulteriore transazione viene scartata e non eseguita
<br>

Le transazioni sono processate da un nodo in _BLOCCHI_. <br>
<<<<<<< HEAD

Ognuno di questi _BLOCCHI_ contiene esattamente _SO_BLOCK_SIZE_ transazioni da processare di cui _SO_BLOCK_SIZE-1_ transazioni ricevute da utenti e una transazione di pagamento per il processing 


# CICLO DI VITA DI UN NODO:
- Creazione di un blocco candidato: <br>
    - Estrazione dalla transaction pool di un insieme massimo di blocchi SO_BLOCK_SIZE transazioni non ancora presenti nel libro mastro

    - Alle transazioni presenti nel blocco, il nodo raggiunge una transazione di reward, con le seguenti caratteristiche:<br>

    * timestamp: il valore attuale di clock_gettime(...)
    * sender : -1 (definire una MACRO...)
    * receiver : l’dentificatore del nodo corrente
    * quantit`a: la somma di tutti i reward delle transazioni incluse nel blocco
    * reward: 0 <br>
---
- Simula l'elaborazione di un blocco attraverso un'attesa non attiva di un intervallo temporale casuale (in nanosecondi) compreso tra SO_MIN_TRANS_PROC_NSEC e SO_MAX_TRANS_PROC_NSEC


----
# LIBRO MASTRO
É struttura condivisa tra tutti i nodi e tutti gli utenti. Si occupa di memorizzare delle transazioni eseguite.<br>
Una transazione si dice confermata solamente quando entra a far parte del libro mastro.
Il libro mastro è formato dalla seguente struttura:

- Una sequenza massima SO_REGISTRY_SIZE di blocchi consecutivi
    
    - All'interno di ogni blocco sono contenute esattamente SO_BLOCK_SIZE transazioni. 

    - Ogni blocco è identificato da un identificatore intero progressivo il cui valore iniziale è imposta
    - Una transazione è identificata da _timestamp_,_sender_,_receiver_.
    Il nodo che aggiunge un nuovo blocco al libro mastro è responsabile dell'aggiornamento dell'identificatore del blocco stesso
=======
>>>>>>> 4e3f7f57f31bdb8da34962cbfc1c3ed19d91342b

Ognuno di questi _BLOCCHI_ contiene esattamente _SO_BLOCK_SIZE_ transazioni da processare di cui _SO_BLOCK_SIZE-1_ transazioni ricevute da utenti e una transazione di pagamento per il processing 

# STAMPA
Ogni secondo il processo master stampa il budget corrente di ogni processo utente e di ongi processo nodo, così come registrato nel libro mastro

<<<<<<< HEAD
# TERMINAZIONE DELLA SIMULAZIONE
Termina se si verfica uno dei seguenti casi:
- sono  trascorsi SO_SIM_SEC_
- il libro mastro si riempie (NB--> il libro può contenere al massimo SO_REGISTRY-SIZE)

Quanod termina il processo master obbliga tutti i processi nodo utente a terminare, e stamperà un riepilogo della simulazione che contiene le seguenti informazioni:
- motivo della terminazione
- bilancio di ogni processo utente, anche quelli terminati prematuramente
- bilancio di ogni processo nodo
- numero dei processi utente terminati prematuramente
- numero di blocchi nel libro mastro 
- per ogni procecsso nodo, numero di transazioni ancora presenti nella transactiom pool
----

---- 
# FINE DELLA PARTE DA 24 
=======
# CICLO DI VITA DI UN NODO:
- Creazione di un blocco candidato: <br>
    - Estrazione dalla transaction pool di un insieme massimo di blocchi SO_BLOCK_SIZE transazioni non ancora presenti nel libro mastro

    - Alle transazioni presenti nel blocco, il nodo raggiunge una transazione di reward, con le seguenti caratteristiche:<br>

    * timestamp: il valore attuale di clock_gettime(...)
    * sender : -1 (definire una MACRO...)
    * receiver : l’dentificatore del nodo corrente
    * quantit`a: la somma di tutti i reward delle transazioni incluse nel blocco
    * reward: 0 <br>
---
- Simula l'elaborazione di un blocco attraverso un'attesa non attiva di un intervallo temporale casuale (in nanosecondi) compreso tra SO_MIN_TRANS_PROC_NSEC e SO_MAX_TRANS_PROC_NSEC
>>>>>>> 4e3f7f57f31bdb8da34962cbfc1c3ed19d91342b

---

----
# LIBRO MASTRO
É struttura condivisa tra tutti i nodi e tutti gli utenti. Si occupa di memorizzare delle transazioni eseguite.<br>
Una transazione si dice confermata solamente quando entra a far parte del libro mastro.
Il libro mastro è formato dalla seguente struttura:

- Una sequenza massima SO_REGISTRY_SIZE di blocchi consecutivi
    
    - All'interno di ogni blocco sono contenute esattamente SO_BLOCK_SIZE transazioni. 

    - Ogni blocco è identificato da un identificatore intero progressivo il cui valore iniziale è imposta
    - Una transazione è identificata da _timestamp_,_sender_,_receiver_.
    Il nodo che aggiunge un nuovo blocco al libro mastro è responsabile dell'aggiornamento dell'identificatore del blocco stesso


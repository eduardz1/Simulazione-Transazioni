# $PROCESSI$ $UTENTE:$

 -Deve esserci un budget iniziale </br>
 Bisogna fare:<br>
 > -le entrare registrate nelle transazioni del libro mastro </br>
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





# $ PROCESSI$ $NODO$


Ogni processo nodo memorizza _privatamente_ la lista di transazioni ricevute da processare (_transaction_pool_)  









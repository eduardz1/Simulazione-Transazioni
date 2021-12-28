void handler(){
    int status;
    int res_kill


    pritnf("ctrl-c killing program");
            for(int i=0;i<SO_NODES_NUMEBR;i++){
                if(childs[i].status==1){
                    res_kills=kill(childs[1].pid, SIGINT);   /* kill all childs*/
        }
    }
    while(wait(&status)!=-1){
        status>>8;
    }
    semctl(semid,0,IPC_RMID);  /*deleting mempid_sem */
    shmtcl(pidmem_id,IPC_RMID,0); /* deleting shared memory segment*/
    exit(1);
}

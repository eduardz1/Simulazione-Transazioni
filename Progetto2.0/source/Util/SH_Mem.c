#include "SH_Mem.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>



void Sh_Mem_Def() {
  /*int shid;*/
  int *shared_data;
  int  
  shid = shmget(IPC_PRIVATE, sizeof(int), S_IRUSR | S_IWUSR);
  if (shid == -1) {
    fprintf(stderr, "%s: %d. Errore in semget #%03d: %s\n", __FILE__, __LINE__,
            errno, strerror(errno));
    exit(EXIT_FAILURE);
  }
  printf("creata e inizializzata l'area di memoria condivisa con ID = %d\n",
         shid);
  shmdt(shared_data);
  exit(EXIT_SUCCESS);
}

void Sh_Attach_Def() { 
  int * shared_data;
  int shid;
  shared_data=shmat(shid, NULL, 0);
  if (errno) {
    fprintf(stderr, "%s: %d. Errore in shmat #%03d: %s\n", __FILE__, __LINE__, errno, strerror(errno));
    exit(EXIT_FAILURE);
 }
   printf("Area di memoria condivisa agganciata.\n");
}

int main(int argc, char const *argv[]) {
  Sh_Mem_Def() ;
  return 0;
}
